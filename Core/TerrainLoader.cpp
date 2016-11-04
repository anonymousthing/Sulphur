#include "TerrainLoader.h"

#include "Platform.h"

#include "Terrain.h"
#include "Vector.h"
#include "Graphics.h"
#include "ResLoader.h"

#include "RAIIHelper.h"

#include <stdint.h>
#include <stdio.h>
#include <random>

struct HeightfieldVertexData {
	//These first 4 fields must be filled out before calling heightfieldToVertices
	float *heightfield;
	int width, height;
	int samplesPerMeter;

	size_t vertexCount;
	size_t vertexBufferSize;
	float *vertices;

	size_t primitiveCount;
	size_t indexCount;
	size_t indexBufferCount;
	uint16_t *indices;
};

static void generateVertices(HeightfieldVertexData *data) {
	//Used here purely for shorthand.
	int width = data->width, height = data->height;

	//Used to determine the maximum size of the vertex buffer.
	size_t vertexCount = width * height;
	size_t vertexBufferSize = vertexCount * 3 * sizeof(float);
	float *vertexBuffer = (float*)malloc(vertexBufferSize);
	float *vertexBufferPos = vertexBuffer;

	float sampleSize = 1.0f / data->samplesPerMeter;
	for (int y = 0; y < height; y++) {
		float scaledY = (float)y * sampleSize;
		for (int x = 0; x < width; x++) {
			float scaledX = (float)x * sampleSize;

			//Here, 'x' and 'y' are 2D coordinates in 'heightField'.
			//We in fact use 'y' as our 3D 'z' and the sample becomes our Y value.
			*vertexBufferPos++ = scaledX; //X
			*vertexBufferPos++ = data->heightfield[y * width + x]; //Y
			*vertexBufferPos++ = scaledY; //Z
		}
	}

	data->vertexCount = vertexCount;
	data->vertexBufferSize = vertexBufferSize;
	data->vertices = vertexBuffer;
}

static void generateIndices(HeightfieldVertexData *data) {
	//Used here purely for shorthand.
	int width = data->width, height = data->height;

	//Used to determine the maximum size of the index buffer.
	size_t primitiveCount = (width - 1) * (height - 1) * 2;
	size_t indexCount = primitiveCount * 3;
	size_t indexBufferSize = indexCount * sizeof(uint16_t);
	uint16_t *indexBuffer = (uint16_t*)malloc(indexBufferSize);
	uint16_t *indexBufferPos = indexBuffer;

	//We generate a triangle list. A triangle strip would work if we didn't
	//do adjustments to compensate for 'feet'. But we do, so it doesn't.
	//Memory @ 4 bytes per index is still better than unindexed triangle lists.
	//Memory @ 2 bytes per index is much better. 16 bit indices can be used for
	//block sizes up to and including 256x256 (2^16 vertices)

	for (int y = 0; y < height - 1; y++) {
		//We are joining vertices from [y] & [y+1]
		for (int x = 0; x < width - 1; x++) {
			//Join [x] & [x+1]

			//Here we decide which diagonal to split from based off the direction that
			//makes less feet.
			float topLeft = data->heightfield[y * width + x];
			float bottomRight = data->heightfield[(y + 1) * width + (x + 1)];
			float tl_br_diff = abs(topLeft - bottomRight);

			float topRight = data->heightfield[y * width + (x + 1)];
			float bottomLeft = data->heightfield[(y + 1) * width + x];
			float tr_bl_diff = abs(topRight - bottomLeft);

			if (tl_br_diff > tr_bl_diff) {
				//A--B
				//| /|
				//|/ |
				//C--D
				*indexBufferPos++ = (y + 0) * width + (x + 0); //A
				*indexBufferPos++ = (y + 0) * width + (x + 1); //B
				*indexBufferPos++ = (y + 1) * width + (x + 0); //C

				*indexBufferPos++ = (y + 0) * width + (x + 1); //B
				*indexBufferPos++ = (y + 1) * width + (x + 1); //D
				*indexBufferPos++ = (y + 1) * width + (x + 0); //C
			} else {
				//A--B
				//|\ |
				//| \|
				//C--D
				*indexBufferPos++ = (y + 0) * width + (x + 0); //A
				*indexBufferPos++ = (y + 0) * width + (x + 1); //B
				*indexBufferPos++ = (y + 1) * width + (x + 1); //D

				*indexBufferPos++ = (y + 0) * width + (x + 0); //A
				*indexBufferPos++ = (y + 1) * width + (x + 1); //D
				*indexBufferPos++ = (y + 1) * width + (x + 0); //C
			}
		}
	}
}

static void heightfieldToVertices(HeightfieldVertexData *data) {
	generateVertices(data);
	generateIndices(data);
}

static VertexArray *verticesToArray(HeightfieldVertexData *data) {
	//TODO: How do we handle indices?
	return nullptr;
}

//weight must be between 0 and 1
template<typename T>
static T lerp(T a, T b, T weight) {
	return ((T)1 - weight) * a + weight * b;
}


//TODO: Consider caching random & engine in some way.
static void createStatic(float *output, int length) {
	std::uniform_real_distribution<float> random;
	std::default_random_engine engine;
	engine.seed(Platform::GetElapsedTime());

	for (int i = 0; i < length; i++) {
		output[i] = random(engine);
	}
}

static void smoothNoise(float *input, int width, int height, int level, float *output) {
	int period = 1 << level;
	float frequency = 1.0f / period;

	for (int y = 0; y < height; y++) {
		int y0 = y - (y % period);
		int y1 = (y0 + period) % height;
		float verticalBlend = (y - y0) * frequency;

		for (int x = 0; x < width; x++) {
			int x0 = x - (x % period);
			int x1 = (x0 + period) % width;
			float horizontalBlend = (x - x0) * frequency;

			float top = lerp(input[y0 * width + x0], input[y0 * width + x1], horizontalBlend);
			float bottom = lerp(input[y1 * width + x0], input[y1 * width + x1], horizontalBlend);
			float final = lerp(top, bottom, verticalBlend);

			output[y * width + x] = final;
		}
	}
}

//Requires output be filled with zeros to begin with
static void generateNoise(float *output, int width, int height, int levels) {
	Buffer<float> noise(width * height);
	Buffer<float> smoothed(width * height);

	createStatic(noise, width * height);
	float weight = 1.0f;
	float totalAmplitude = 1.0f;
	
	for (int level = levels - 1; level >= 0; level--) {
		weight *= 0.5f;
		totalAmplitude += weight;

		smoothNoise(noise, width, height, level, smoothed);
		for (int y = 0; y < height; y++) {
			for (int x = 0; x < width; x++) {
				output[y * width + x] += (smoothed[y * width + x] * weight);
			}
		}
	}

	//Adjust for amplitude
	totalAmplitude = 1.0f / totalAmplitude;
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			output[y * width + x] *= totalAmplitude;
		}
	}
}


//Quantise noise suitable for outputting to a bitmap.
static void quantiseNoise(float *noise, int length, uint8_t *output, uint8_t divisions = 255) {
	float multiplier = 255.0f / divisions;
	for (int i = 0; i < length; i++) {
		output[i] = (int)(((int)(noise[i] * divisions)) * multiplier);
	}
}

//Calculate the rounded row-size
static int bitmapRowSize(int width, int bytesPerPixel) {
	int size = width * bytesPerPixel;
	//Round up to multiple of 4
	if (size % 4 != 0) {
		size = (size / 4 + 1) * 4;
	}
	return size;
}

//Write a bitmap header to file.
static bool writeBitmapHeader(FILE *file, int width, int height, int bytesPerPixel) {
	const int bmpHeaderSize = 0x36;

#pragma pack(push)
#pragma pack(1)
	struct {
		uint16_t magic;
		uint32_t fileSize;
		uint32_t zero;
		uint32_t offsetToData;
		uint32_t headerSize;
		uint32_t width, height;
		uint16_t colorPlaneCount;
		uint16_t bitsPerPixel;
		uint32_t compression;
		uint32_t imageDataSize;
		uint32_t hPixelsPerMeter, vPixelsPerMeter;
		uint32_t colorsInPalette, importantColorCount;
	} bmpHeader;
#pragma pack(pop)

	bmpHeader.magic = 0x4D42; //BM
	bmpHeader.imageDataSize = bitmapRowSize(width, bytesPerPixel) * height;
	bmpHeader.fileSize = bmpHeader.imageDataSize + bmpHeaderSize;
	bmpHeader.zero = 0;
	bmpHeader.offsetToData = bmpHeaderSize;
	bmpHeader.headerSize = 40;
	bmpHeader.width = width;
	bmpHeader.height = height;
	bmpHeader.colorPlaneCount = 1;
	bmpHeader.bitsPerPixel = bytesPerPixel * 8;
	bmpHeader.compression = 0;
	bmpHeader.hPixelsPerMeter = 1000;
	bmpHeader.vPixelsPerMeter = 1000;
	bmpHeader.colorsInPalette = 0;
	bmpHeader.importantColorCount = 0;

	if (fwrite(&bmpHeader, 1, sizeof(bmpHeader), file) != sizeof(bmpHeader)) {
		return false;
	} else {
		return true;
	}
}

//Writes a 24bpp bitmap
static bool writeBitmap(const char *name, int width, int height, uint8_t *r, uint8_t *g, uint8_t *b) {
	FileHandle file = fopen(name, "wb");
	if (file == nullptr) {
		return false;
	}

	if (!writeBitmapHeader(file, width, height, 3)) {
		return false;
	}

	//Padding needed for every row
	int rowPadding = bitmapRowSize(width, 3) - width * 3;

	//Write image data
	for (int y = height - 1; y >= 0; y--) {
		for (int x = 0; x < width; x++) {
			fputc(r[y * width + x], file);
			fputc(g[y * width + x], file);
			fputc(b[y * width + x], file);
		}
		//Write Padding
		for (int i = 0; i < rowPadding; i++) {
			fputc(0, file);
		}
	}

	return true;
}

void testNoise() {
	int width = 2048, height = 2048;
	int levels = 7;

	Buffer<float> rawNoise(width * height);
	memset(rawNoise, 0, sizeof(float) * width * height);
	generateNoise(rawNoise, width, height, levels);

	Buffer<uint8_t> quantised(width * height);
	quantiseNoise(rawNoise, width * height, quantised);

	writeBitmap("__TestNoise.bmp", width, height, quantised, quantised, quantised);

	Buffer<float> wavelet(width * height);
	createStatic(rawNoise, width * height);

	quantiseNoise(rawNoise, width * height, quantised);
	writeBitmap("__Static.bmp", width, height, quantised, quantised, quantised);
}


Terrain *TerrainLoader::loadTerrain(const char *terrainFile) {
	TextureHandle grass = ResLoader::loadTexture("grass.dds");



	return nullptr;
}


