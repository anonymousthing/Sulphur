#include "UTF8Helper.h"

//TODO: This should probably use the definition from Core/Font.h
static const uint32_t invalidCodePoint = 0x0000007F;

static void skipToValidCodePoint(const char *str, size_t *position) {
	//Skip to the next location that is does not begin with 0b10.....
	//Also do not allow 0b1111111x
	uint8_t current;

	//Stop if we reach a zero.
	while (current = str[*position]) {
		if ((current & 0xC0) != 0x80) {
			//We're not a continuation character
			if ((current & 0xFE) != 0xFE) {
				//And we're not too big!
				break;
			}
		}

		//Next character
		(*position)++;
	}

}

uint32_t UTF8Helper::extractNextCodePoint(const char *str, size_t *position) {
	uint8_t first = str[*position];
	if (first == 0) {
		return 0;
	}

	(*position)++;

	//Simple case, single char
	if ((first & 0x80) == 0) { //0b10000000
		return first;
	}

	//Code point certainly too high
	if ((first & 0xFE) == 0xFE) { //0b1111111x -- Invalid codepoint
		skipToValidCodePoint(str, position);
		return invalidCodePoint;
	}

	//The number of expected additional bytes.
	//TODO: Look at doing this using intrinsics
	int expectedNumBytes = 0;
	uint32_t codepoint = 0;
	if ((first & 0xFE) == 0xFC) {			//first & 0b11111110 == 0b1111110x
		codepoint = first & 0x01;		//0b00000001
		expectedNumBytes = 5;
	} else if ((first & 0xFC) == 0xF8) {	//first & 0b11111100 == 0b111110xx
		codepoint = first & 0x03;		//0b00000011
		expectedNumBytes = 4;
	} else if ((first & 0xF8) == 0xF0) {	//first & 0b11111000 == 0b11110xxx
		codepoint = first & 0x07;		//0b00000111
		expectedNumBytes = 3;
	} else if ((first & 0xF0) == 0xE0) {	//first & 0b11110000 == 0b1110xxxx
		codepoint = first & 0x0F;		//0b00001111
		expectedNumBytes = 2;
	} else if ((first & 0xE0) == 0xC0) {	//first & 0b11100000 == 0b110xxxxx
		codepoint = first & 0x1F;		//0b00011111
		expectedNumBytes = 1;
	} else {
		skipToValidCodePoint(str, position);
		return invalidCodePoint;
	}

	for (int i = 0; i < expectedNumBytes; i++) {
		uint8_t current = str[*position];
		if (current == '\0') {
			//End of string mid-codepoint.
			return invalidCodePoint;
		}

		if ((current & 0xC0) == 0x80) { //Check that current is 0b10xxxxxx
			codepoint <<= 6;
			codepoint |= (current & 0x3F); //0b00111111
		} else {
			skipToValidCodePoint(str, position);
			return invalidCodePoint;
		}

		//Consume this byte
		(*position)++;
	}

	return codepoint;
}

