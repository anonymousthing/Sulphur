#include "FontLoader.h"

#include "Log.h"
#include "Font.h"
#include "ResLoader.h"
#include "RAIIHelper.h"

#include <stdio.h>
#include <ctype.h>

static Log logger("FontLoader");

static void removeTrailingSpace(char *line, size_t len) {
	//buffEnd points to the last char before the trailing \0
	char *buffEnd = line + len - 1;
	while (isspace(*buffEnd)) {
		*buffEnd = '\0';
		buffEnd--;
	}
}

static char *removeLeadingSpace(char *line) {
	//Remove leading space
	while (*line && isspace(*line))
		line++;
	return line;
}

static char *getLine(FILE *file, char *fileBuffer, size_t buffSize, int *lineNum) {
	while (char *buff = fgets(fileBuffer, (int)buffSize, file)) {
		++(*lineNum);

		//Check for and remove comments
		char *temp = buff;
		while (*temp) {
			if (*temp == '#') {
				*temp = '\0';
				break;
			}
			temp++;
		}

		buff = removeLeadingSpace(buff);

		size_t len = strlen(buff);
		if (len == 0) {
			continue;
		}

		removeTrailingSpace(buff, len);

		//We now have a trimmed line.
		//len is trimmed length, which can never be 0 at this point.
		return buff;
	}
	return nullptr;
}

static char *isFontPlane(char *line) {
	if (*line == '[') {
		size_t len = strlen(line);
		if (line[len - 1] == ']') {
			//Trim leading '[' & trailing ']'
			line[len - 1] = '\0';
			line++;
			return line;
		}
	}

	return nullptr;
}

//Returns the part after the '='.
//line is altered to be null terminated after value.
static char *splitValue(char *line) {
	char *value = line;

	//Seek to *value == '='
	while (*value && *value != '=') {
		value++;
	}

	if (*value != '=') {
		//No value found
		return nullptr;
	}

	*value = '\0';
	value++;

	removeTrailingSpace(line, strlen(line));
	value = removeLeadingSpace(value);

	if (*value == '\0') {
		//No value
		return nullptr;
	}

	return value;
}

static bool matches(const char *value, const char *str) {
	size_t len = strlen(str);
	return strncmp(value, str, len + 1) == 0;
}

static bool parseValue(const char *src, int *value, int radix = 10) {
	char *end = nullptr;
	long temp = strtol(src, &end, radix);

	if (*end != '\0') {
		return false;
	}

	//Range check
	if (temp > INT_MAX || temp < 0) {
		return false;
	}

	*value = (int)temp;
	return true;
}

static bool parseRange(char *values, std::vector<CodePointRange> &codepoints) {
	//Parse range of form
	//0x23-0x24,0x52,0xFFF-0x1FFF

	char *nextSegment = values;
	while (*nextSegment) {
		values = nextSegment;

		char *value2 = nullptr;
		//Find & split next comma
		while (*nextSegment) {
			//Prepare 2nd vlaue
			if (*nextSegment == '-') {
				*nextSegment = '\0';
				value2 = nextSegment + 1;
			} else if (*nextSegment == ',') {
				*nextSegment = '\0';
				nextSegment++;
				break;
			}
			nextSegment++;
		}

		//When parsing values below, we allo strtol to detect prefixes.

		int a, b;
		if (!parseValue(values, &a, 0)) {
			return false;
		}

		if (value2 != nullptr) {
			//2 values
			if (!parseValue(value2, &b, 0)) {
				return false;
			}

			//Ensure b >= a
			if (a > b) {
				return false;
			}

			codepoints.push_back(CodePointRange(a, b));
		} else {
			//1 value
			codepoints.push_back(CodePointRange(a, a));
		}
	}

	return true;
}

Font *FontLoader::loadFont(const char *fontDescriptionFile) {
	FileHandle file = fopen(fontDescriptionFile, "r");
	if (file == nullptr) {
		logger.error("Unable to open font file \"%s\"", fontDescriptionFile);
		return nullptr;
	}
	logger.info("Loading font file \"%s\"", fontDescriptionFile);

	Buffer<char, 512> lineBuff;
	char *line;
	int lineNum = 0;

	std::vector<FontPlane> planes;

	//Temp data associated with a FontPlane under construction.
	TextureHandle texture;
	int codepointsPerLine = 0, linesPerTexture = 0;

	bool hadCPL = false, hadLPT = false;
	bool inFontPlane = false;
	std::vector<CodePointRange> codePoints;

	while (line = getLine(file, lineBuff, 512, &lineNum)) {
		//line is trimmed and free from comments.

		if (char *fontPlaneName = isFontPlane(line)) {
			if (inFontPlane) {
				//Save current fontPlane
				if (!hadCPL || !hadLPT || codePoints.size() == 0) {
					logger.error("Font description was missing fields, line %i", lineNum);
					return nullptr;
				}

				FontPlane plane(texture, codePoints, codepointsPerLine, linesPerTexture);
				planes.push_back(std::move(plane));

				//Reset info
				codePoints.clear();
				hadCPL = hadLPT = false;
			}

			texture = ResLoader::loadTexture(fontPlaneName);
			if (!texture) {
				logger.error("Unable to load font due to previous failure.");
				return nullptr;
			}
			inFontPlane = true;

			continue;
		}
		if (!inFontPlane) {
			logger.error("Invalid data on line %i", lineNum);
			return nullptr;
		}

		char *value = splitValue(line);
		if (value == nullptr) {
			logger.error("Unable to get value from line %i", lineNum);
			return nullptr;
		}

		if (matches(line, "Range")) {
			if (!parseRange(value, codePoints)) {
				logger.error("Error parsing value \"%s\" on line %i", value, lineNum);
				return nullptr;
			}
		} else {
			int parsedValue;
			if (!parseValue(value, &parsedValue)) {
				logger.error("Error parsing value \"%s\" on line %i", value, lineNum);
				return nullptr;
			}

			if (matches(line, "GridWidth")) {
				//Ignore
			} else if (matches(line, "GridHeight")) {
				//Ignore
			} else if (matches(line, "CharsPerLine")) {
				hadCPL = true;
				codepointsPerLine = parsedValue;
			} else if (matches(line, "LinesPerTexture")) {
				hadLPT = true;
				linesPerTexture = parsedValue;
			} else {
				logger.error("Unknown field \"%s\" on line %i", line, lineNum);
				return nullptr;
			}
		}
	}
	if (inFontPlane) {
		FontPlane plane(texture, codePoints, codepointsPerLine, linesPerTexture);
		planes.push_back(std::move(plane));
	}
	//TODO: Check for 0x7F
	return new Font(std::move(planes));
}

