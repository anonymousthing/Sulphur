#pragma once

#include <stdint.h>
#include <vector>

#include "Vector.h"
#include "VertexArray.h"
#include "GraphicsHandles.h"
#include "Sprite.h"

class Graphics;

//Every font must contain a definition for this character, as it is used for error recovery.
#define FONT_INVALID_CODEPOINT	(0x0000007F)

struct CodePointRange;

class FontPlane {
private:
	TextureHandle texture;

	//Used to calculate centrepoints.
	int codepointsPerLine, linesPerTexture;

	int codepointRangeCount;
	CodePointRange *codepoints;

	int indexOf(uint32_t codepoint) const;

	void getPositionalBounds(int codepointIndex, Vector4 *positionalBounds) const;
	void getTextureBounds(int codepointIndex, Vector4 *textureBounds) const;

public:
	//FontPlane owns the given TextureHandle and will dispose of it.
	FontPlane(TextureHandle texture, const std::vector<CodePointRange> &codepoints, int codepointsPerLine, int linesPerTexture);
	~FontPlane();

	FontPlane(FontPlane &&other);
	FontPlane &operator=(FontPlane &&other);

	FontPlane(const FontPlane &other) = delete;
	FontPlane &operator=(const FontPlane &other) = delete;

	//This function returns 0 if the codepoint is not, else an int suitable
	//for passing to other FontPlane functions to further query the given
	//codepoint.
	int containsCodepoint(uint32_t codepoint) const {
		return indexOf(codepoint) + 1;
	}

	Vector4 getPositionalBounds(int codepointIndex) const {
		Vector4 bounds;
		getPositionalBounds(codepointIndex - 1, &bounds);
		return bounds;
	}

	Vector4 getTextureBounds(int codepointIndex) const {
		Vector4 bounds;
		getTextureBounds(codepointIndex - 1, &bounds);
		return bounds;
	}

	void getBounds(int codepointIndex, Vector4 *positionalBounds, Vector4 *textureBounds) const {
		getPositionalBounds(codepointIndex - 1, positionalBounds);
		getTextureBounds(codepointIndex - 1, textureBounds);
	}

	TextureHandle getTexture() const { return texture; }
};

struct FontFixedString {
	std::vector<Sprite> parts;

	FontFixedString() { }
	FontFixedString(FontFixedString &&other) : parts(std::move(other.parts)) { }

	FontFixedString &operator=(FontFixedString &&other) {
		parts = std::move(other.parts);
		return (*this);
	}

	FontFixedString(const FontFixedString &other) = delete;
	FontFixedString &operator=(const FontFixedString &other) = delete;
};

class Font {

private:
	std::vector<FontPlane> fontPlanes;

	float addCharToBuffer(const FontPlane *plane, int codepointIndex, float baseX, float baseY, float **positionBuff, float **texBuff);
	
public:
	Font(std::vector<FontPlane> &&fontPlanes)
		: fontPlanes(std::move(fontPlanes)) {
	}

	//void render(const char *str, Vector2 screenBase);

	FontFixedString createFixedString(Graphics *graphics, const char *str, Vector2 screenBase);
	void drawFixedString(Graphics *graphics, const FontFixedString &str);
};

struct CodePointRange {
	uint32_t low, high;

	inline uint32_t range() const {
		return (high - low) + 1;
	}
	
	//Returns 0 if not found, else (index+1)
	inline int contains(uint32_t codepoint) const {
		if (low <= codepoint && high >= codepoint) {
			return (codepoint - low) + 1;
		}

		return 0;
	}

	CodePointRange(uint32_t low, uint32_t high)
		: low(low), high(high) {
	}

	//Move constructor
	CodePointRange(CodePointRange &&other) {
		low = other.low;
		high = other.high;
	}

	//Move assignment operator
	CodePointRange &operator=(CodePointRange &&other) {
		low = other.low;
		high = other.high;
	}

	//Less operator
	bool operator <(const CodePointRange &other) const {
		return low < other.low;
	}
};
