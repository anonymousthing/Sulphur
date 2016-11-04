#pragma once

class Font;

class FontLoader {
public:
	static Font *loadFont(const char *fontDescriptionFile);
};

