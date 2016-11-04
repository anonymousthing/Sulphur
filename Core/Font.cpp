#include "Font.h"

#include "Graphics.h"
#include "GraphicsLoader.h"
#include "GLHandles.h"

#include "Matrix.h"
#include "UTF8Helper.h"
#include "Log.h"

#include <string.h>
#include <vector>

static Log logger("Font");

FontPlane::FontPlane(TextureHandle texture, const std::vector<CodePointRange> &srcCodepoints, int codepointsPerLine, int linesPerTexture)
	: texture(texture), codepointsPerLine(codepointsPerLine), linesPerTexture(linesPerTexture) {

	size_t size = sizeof(CodePointRange) * srcCodepoints.size();
	codepoints = (CodePointRange*)malloc(size);
	codepointRangeCount = (int)srcCodepoints.size();
	memcpy(codepoints, srcCodepoints.data(), size);
}

FontPlane::~FontPlane() {
	if (texture) {
		releaseHandle(texture);
	}

	free(codepoints);
}

FontPlane::FontPlane(FontPlane &&other) {
	texture = other.texture;
	codepointsPerLine = other.codepointsPerLine;
	linesPerTexture = other.linesPerTexture;
	codepointRangeCount = other.codepointRangeCount;
	codepoints = other.codepoints;

	other.codepointRangeCount = 0;
	other.codepoints = nullptr;
	other.texture = nullptr;
}

FontPlane &FontPlane::operator=(FontPlane &&other) {
	texture = other.texture;
	codepointsPerLine = other.codepointsPerLine;
	linesPerTexture = other.linesPerTexture;
	codepointRangeCount = other.codepointRangeCount;
	codepoints = other.codepoints;

	other.codepointRangeCount = 0;
	other.codepoints = nullptr;
	other.texture = nullptr;
	return (*this);
}

int FontPlane::indexOf(uint32_t codepoint) const {
	int cumlativeIndex = 0;

	for (int i = 0; i < codepointRangeCount; i++) {
		const CodePointRange &current = codepoints[i];

		if (int offset = current.contains(codepoint)) {
			return cumlativeIndex + offset - 1;
		} else {
			cumlativeIndex += current.range();
		}
	}

	return -1;
}

void FontPlane::getPositionalBounds(int codepointIndex, Vector4 *positionalBounds) const {
	//TODO: Variable width characters
	float width = 2.0f / 52.0f;
	float x = 0;
	float height = 2.0f / 13.0f;
	float y = 0;

	positionalBounds->x() = x;
	positionalBounds->y() = y;
	positionalBounds->z() = x + width;
	positionalBounds->w() = y + height;
}

void FontPlane::getTextureBounds(int codepointIndex, Vector4 *textureBounds) const {
	//TODO: Variable width characters
	int row = codepointIndex / codepointsPerLine;
	int col = codepointIndex % codepointsPerLine;

	float width = 1.0f / (float)codepointsPerLine;
	float x = width * col;
	float height = 1.0f / (float)linesPerTexture;
	float y = 1.0f - (height * row);

	textureBounds->x() = x;
	textureBounds->y() = y - height;
	textureBounds->z() = x + width;
	textureBounds->w() = y;
}

//Stored { left, top, right, bottom }
static void addQuad(float **buffer, Vector4 bounds) {
	//Two triangles
	//1: 0,0 1,0 0,1
	//2: 1,0 1,1 0,1

	//As triangle list 0,0 1,0 0,1 1,1

	float *buffPos = *buffer;

	//First triangle
	buffPos[0] = bounds.x();
	buffPos[1] = bounds.y();

	buffPos[2] = bounds.z();
	buffPos[3] = bounds.y();

	buffPos[4] = bounds.x();
	buffPos[5] = bounds.w();

	//Second triangle
	buffPos[6] = bounds.z();
	buffPos[7] = bounds.w();

	//prims * verts
	(*buffer) += (4 * 2);
}

float Font::addCharToBuffer(const FontPlane *plane, int codepointIndex, float baseX, float baseY, float **positionBuff, float **texBuff) {
	//Output format is 4 floats per vertex, (x, y, u, v) and 3 verts per primitive.

	Vector4 position, texture;
	plane->getBounds(codepointIndex, &position, &texture);
	position.x() += baseX;
	position.y() += baseY;
	position.z() += baseX;
	position.w() += baseY;

	addQuad(positionBuff, position);
	addQuad(texBuff, texture);

	float characterWidth = position.z() - position.x();
	return characterWidth;
}

//The address of the found FontPlane is returned through *fontPlane, and the index is
//given through the return value. This function always succeeds, except when the current
//Font does not contain a definition for FONT_INVALID_CODEPOINT, in which case this
//function will terminate the application. Note that it is supposed to be impossible to
//load a font that is invalid in this way.
static int findCodePoint(const std::vector<FontPlane> &planes, uint32_t codepoint, const FontPlane **fontPlane) {
	for (auto pi = planes.begin(), pe = planes.end(); pi != pe; ++pi) {
		const FontPlane &plane = *pi;

		if (int index = plane.containsCodepoint(codepoint)) {
			*fontPlane = &plane;
			return index;
		}
	}

	//Not found. We don't care about performance here, this shouldn't happen
	//and if it does, let them suffer a low framerate >:D

	if (codepoint == FONT_INVALID_CODEPOINT) {
		logger.fatal("Could not find codepoint 0x%.8x, which is required to be present "
			"in every font for error recovery purposes.", codepoint);
		return 0; //fatal never returns.
	} else {
		return findCodePoint(planes, FONT_INVALID_CODEPOINT, fontPlane);
	}
}

//TODO: Do we want to support \n as an actual newline? Currently it is just another renderable character.
FontFixedString Font::createFixedString(Graphics *graphics, const char *str, Vector2 screenBase) {
	//Create an empty FixedString
	FontFixedString ffs;

	//len is the maximum number of code points in str.
	//This is used to allocate buffers, given that it is a safe maximum.
	size_t len = strlen(str);
	if (len == 0) {
		return ffs;
	}

	//Fixed Font String Construction Buffer.
	//Holds junk about each piece of a FFS while we build it.
	struct FFSCB {
		float *vertexBuffer, *textureBuffer;
		float *vBufferPos, *tBufferPos;
		const FontPlane *originatingPlane;
	};
	std::vector<FFSCB> segments;
	FFSCB *currentSegment = nullptr;

	//Our current x & y co-ordinates.
	float x = screenBase[0], y = screenBase[1];

	size_t position = 0; //Position in str.

	//The current character's FontPlane, cached outside the loop to make
	//successive access to the same plane faster.
	const FontPlane *plane = nullptr;
	while (uint32_t codepoint = UTF8Helper::extractNextCodePoint(str, &position)) {
		int codepointIndex = 0;

		//This check makes it faster on larger fonts to check for runs of
		//characters that come from the same plane.
		if (!plane || !(codepointIndex = plane->containsCodepoint(codepoint))) {
			codepointIndex = findCodePoint(fontPlanes, codepoint, &plane);

			//Chase up a new currentSegment. First off just check for an existing one.
			currentSegment = nullptr;
			for (auto si = segments.begin(), se = segments.end(); si != se; ++si) {
				FFSCB &segment = *si;

				if (segment.originatingPlane == plane) {
					currentSegment = &segment;
					break;
				}
			}

			//If we didn't find one matching current plane, create a new one.
			if (currentSegment == nullptr) {
				segments.push_back(FFSCB());
				currentSegment = &segments.back();

				currentSegment->originatingPlane = plane;

				//Output format is 4 floats per vertex, (x, y, u, v) and 4 verts per char
				//Therefore the maximum size is (sizeof(float) * 2 * 4 * len) * 2 buffers (1 pos, 1 tex)
				currentSegment->vertexBuffer = (float*)malloc(sizeof(float) * 2 * 4 * len);
				currentSegment->vBufferPos = currentSegment->vertexBuffer;

				currentSegment->textureBuffer = (float*)malloc(sizeof(float) * 2 * 4 * len);
				currentSegment->tBufferPos = currentSegment->textureBuffer;
			}
		}

		//Add to the current buffer and move right.
		x += addCharToBuffer(plane, codepointIndex, x, y, &currentSegment->vBufferPos, &currentSegment->tBufferPos);
	}

	//Whew... That took ages...
	//Time to go through segments and fill in the FontFixedString
	bool error = false;
	for (auto si = segments.begin(), se = segments.end(); si != se; ++si) {
		FFSCB &segment = *si;

		if (!error) {
			size_t difference = segment.vBufferPos - segment.vertexBuffer;
			//We had difference floats @ 2 floats per vertex.
			size_t verticeCount = difference / 2;

			TextureHandle texture = segment.originatingPlane->getTexture();
			VertexArray *vertices = GraphicsLoader::generateVertices((int)verticeCount, segment.vertexBuffer, 2, segment.textureBuffer, 2);

			if (vertices == nullptr) {
				error = true;
				continue;
			}

			Sprite sprite(vertices, new Texture(texture), 0, 0, graphics);
			sprite.setOwnsTexture(false);
			ffs.parts.push_back(std::move(sprite));
		}

		free(segment.vertexBuffer);
		free(segment.textureBuffer);
	}

	if (error) {
		//This will free any resources we allocated.
		ffs.parts.clear();
	}

	return ffs;
}

void Font::drawFixedString(Graphics *graphics, const FontFixedString &str) {
	GLuint shader = 0;
	Matrix4x4 matrix = Matrix4x4::Identity();

	for (auto pi = str.parts.begin(), pe = str.parts.end(); pi != pe; ++pi) {
		const Sprite &sprite = *pi;

		graphics->render2d(sprite.vertices, sprite.texture->getHandle(), Vector2(0, 0));
	}
}
