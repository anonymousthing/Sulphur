#include "ResLoader.h"
#include "GLHandles.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include <string>
#include <fstream>
#include <algorithm>
#include <vector>

#include <png.h>
#include <zlib.h>

//TODO: Do we want to abstract this to "Platform"?
//It works on Windows, Mac & Linux so can we just pretend it's "standard"?
#include <sys/stat.h>

#define FOURCC_DXT1			0x31545844
#define FOURCC_DXT3			0x33545844
#define FOURCC_DXT5			0x35545844

#define MAX_TEX_DIMENSION	65536		//The maxiumum dimension for any texture

#define DDS_MAGIC			0x20534444	//ASCII "DDS "
#define DDS_MAGIC_MASK		0xFFFFFFFF

#define PNG_MAGIC			0x474E5089
#define PNG_MAGIC_MASK		0xFFFFFFFF

#define BMP_MAGIC			0x00004D42
#define BMP_MAGIC_MASK		0x0000FFFF

static Log logger("ResLoader");

//See: http://msdn.microsoft.com/en-us/library/bb943984.aspx
static struct DDSPixelFormat {
	uint32_t	size;			//Must equal 32 (sizeof(DDSPixelFormat))
	uint32_t	flags;			//0x04 should be set, indicating fourcc is set.
	uint32_t	fourCC;			//Must be one of the above FOURCC macros.
	uint32_t	rgbBitCount;	//
	uint32_t	rBitMask;		//
	uint32_t	bBitMask;		//
	uint32_t	gBitMask;		//
	uint32_t	aBitMask;		//
};

//See: http://msdn.microsoft.com/en-us/library/bb943982.aspx
static struct DDSHeader {
	uint32_t		size;				//Must equal 124
	uint32_t		flags;				//Must have 0x1, 0x2, 0x4, 0x100 (every DDS). 
	uint32_t		height;				//
	uint32_t		width;				//
	uint32_t		pitchOrLinearSize;	//
	uint32_t		depth;				//For volume textures (DDSD_DEPTH)
	uint32_t		mipMapCount;		//
	uint32_t		reserved1[11];		//
	DDSPixelFormat	pixelFormat;		//
	uint32_t		caps;				//
	uint32_t		caps2;				//
	uint32_t		caps3;				//
	uint32_t		caps4;				//
	uint32_t		reserved2;			//
};

TextureHandle ResLoader::loadDDS(const char *ddsPath){
	FILE *file = fopen(ddsPath, "rb");
	if (file == NULL) {
		logger.error("Could not open file.");
		return 0;
	}

	//Check DDS magic
	int magic;
	if (fread(&magic, 1, 4, file) != 4 || magic != DDS_MAGIC) {
		if (magic != DDS_MAGIC) {
			logger.error("Invalid DDS Magic");
		} else {
			logger.error("IO Error reading DDS Magic.");
		}

		fclose(file);
		return 0;
	}

	//Load the header
	DDSHeader header;
	if (fread(&header, 1, sizeof(header), file) != sizeof(header)) {
		logger.error("IO Error reading DDS header.");
		fclose(file);
		return 0;
	}

	//FIXME: More validation of DDS header?
	//Check that size, pixelFormat.size, flags, pixelFormat.flags, etc are all valid.

	uint32_t width = header.width, height = header.height;

	//Enforce maximum dimension (to stop us from running out of memory)
	if (width > MAX_TEX_DIMENSION || height > MAX_TEX_DIMENSION) {
		logger.error("Texture too large.");
		fclose(file);
		return 0;
	}

	//DXT stride & format
	uint32_t components = (header.pixelFormat.fourCC == FOURCC_DXT1) ? 3 : 4;
	uint32_t format;
	uint32_t dxtStride;
	switch (header.pixelFormat.fourCC) {
		case FOURCC_DXT1:
			format = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
			dxtStride = 8;
			break;
		case FOURCC_DXT3:
			format = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
			dxtStride = 16;
			break;
		case FOURCC_DXT5:
			format = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
			dxtStride = 16;
			break;
		default:
			logger.error("Unrecognised pixelFormat.fourCC.");
			return 0;
	}

	//Create texture
	GLuint tID;
	glGenTextures(1, &tID);
	glBindTexture(GL_TEXTURE_2D, tID);

	//See: http://msdn.microsoft.com/en-us/library/bb205578.aspx
	size_t maxBuffSize = std::max(1U, (width + 3) / 4) * std::max(1U, (height + 3) / 4) * dxtStride;
	uint8_t *buffer = (uint8_t*)malloc(maxBuffSize);
	//mipWidth/mipHeight always decrease, therefore this is always big enough.

	//Load MipMaps
	uint32_t mipWidth = width, mipHeight = height;
	for (uint32_t i = 0; i < header.mipMapCount; i++) {
		uint32_t mipMapSize = std::max(1U, (mipWidth + 3) / 4) * std::max(1U, (mipHeight + 3) / 4) * dxtStride;

		//Read this mip level
		if (fread(buffer, 1, mipMapSize, file) != mipMapSize) {
			logger.error("IO Error reading DDS data.");
			fclose(file);
			glDeleteTextures(1, &tID);
			free(buffer);
			return 0;
		}

		glCompressedTexImage2D(GL_TEXTURE_2D, i, format, mipWidth, mipHeight, 0, mipMapSize, buffer);

		mipWidth /= 2;
		mipHeight /= 2;
		if (mipWidth == 0 || mipHeight == 0) {
			break;
		}
	}
	free(buffer);

	//Finished with file
	fclose(file);

	return encodeHandle<TextureHandle>(tID);
}

TextureHandle ResLoader::loadPNG(const char *path) {
	//Initialise PNG control structure
	png_image image;
	memset(&image, 0, sizeof(image));
	image.version = PNG_IMAGE_VERSION;

	//TODO: Error checking (image.warning_or_error, see http://www.libpng.org/pub/png/libpng-manual.txt)
	if (!png_image_begin_read_from_file(&image, path)) {
		return nullptr;
	}

	png_bytep buffer;
	image.format = PNG_FORMAT_RGB;
	
	if (image.width > MAX_TEX_DIMENSION || image.height > MAX_TEX_DIMENSION) {
		logger.error("Image is too big to be loaded");
		return nullptr;
	}

	size_t imgSize = PNG_IMAGE_SIZE(image);
	buffer = (png_bytep)malloc(imgSize);

	if (!buffer || !png_image_finish_read(&image, nullptr, buffer, 0, nullptr)) {
		logger.error("Unknown error reading .png");
		free(buffer);
		return nullptr;
	}

	GLuint texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.width, image.height, 0, GL_RGB, GL_UNSIGNED_BYTE, buffer);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	return encodeHandle<TextureHandle>(texture);
}

TextureHandle ResLoader::loadTexture(const char *path) {
	FILE *file = fopen(path, "rb");

	uint32_t magic;
	if (fread(&magic, 1, 4, file) != 4) {
		logger.error("Failed to open file \"%s\"", path);
		fclose(file);
		return nullptr;
	}
	fclose(file); //File not needed

	if ((magic & PNG_MAGIC_MASK) == PNG_MAGIC) {
		logger.info("Image \"%s\" detected as PNG", path);
		return loadPNG(path);
	} else if ((magic & DDS_MAGIC_MASK) == DDS_MAGIC) {
		logger.info("Image \"%s\" detected as DDS", path);
		return loadDDS(path);
	} else if ((magic & BMP_MAGIC_MASK) == BMP_MAGIC) {
		logger.error("Image \"%s\" detected as BMP -- but BMP is unsupported.", path);
		return nullptr;
	} else {
		logger.error("Unable to detect format of \"%s\"", path);
		return nullptr;
	}
}

#define	MAX_FILE_SIZE	(1024*1024*10)	//Files bigger than 10MB are 'too big' to be loaded.
										//This only applies to shaders.

static size_t bufferFile(const char *filePath, uint8_t **buff) {
	//FIXME: Technically speaking this is a race condition. Should we lock the file first? (Do we care?)

	//Query fileSize
	struct stat attributes;
	if (stat(filePath, &attributes) != 0) {
		return -1;
	}

	size_t fileSize = (size_t)attributes.st_size;
	if (fileSize >= MAX_FILE_SIZE) {
		return -2;
	}

	//Open the file
	FILE *file;
	if ((file = fopen(filePath, "rb")) == NULL) {
		return -1;
	}

	//Allocate a buffer big enough for the entire file & read it into memory.
	*buff = (uint8_t*)malloc(fileSize + 1);
	if (fread(*buff, 1, fileSize, file) != fileSize) {
		fclose(file);
		free(buff);
		return -1;
	}
	(*buff)[fileSize] = 0; //null terminate our buffer.
	fclose(file);

	return fileSize;
}

GLuint ResLoader::loadBMP_custom(const char *imagepath) {
	unsigned char header[54]; // Each BMP file begins by a 54-bytes header
	unsigned int dataPos;     // Position in the file where the actual data begins
	unsigned int width, height;
	unsigned int imageSize;   // = width*height*3
	// Actual RGB data

	unsigned char * data;
	// Open the file
	FILE * file = fopen(imagepath, "rb");
	if (!file)
	{
		printf("Image could not be opened\n"); return 0;
	}

	if (fread(header, 1, 54, file) != 54){ // If not 54 bytes read : problem
		printf("Not a correct BMP file\n");
		return false;
	}
	if (header[0] != 'B' || header[1] != 'M'){
		printf("Not a correct BMP file\n");
		return 0;
	}
	// Read ints from the byte array
	dataPos = *(int*)&(header[0x0A]);
	imageSize = *(int*)&(header[0x22]);
	width = *(int*)&(header[0x12]);
	height = *(int*)&(header[0x16]);

	// Some BMP files are misformatted, guess missing information
	if (imageSize == 0)    imageSize = width*height * 3; // 3 : one byte for each Red, Green and Blue component
	if (dataPos == 0)      dataPos = 54; // The BMP header is done that way

	// Create a buffer
	data = new unsigned char[imageSize];

	// Read the actual data from the file into the buffer
	fread(data, 1, imageSize, file);

	//Everything is in memory now, the file can be closed
	fclose(file);

	// Create one OpenGL texture
	GLuint textureID;
	glGenTextures(1, &textureID);

	// "Bind" the newly created texture : all future texture functions will modify this texture
	glBindTexture(GL_TEXTURE_2D, textureID);

	// Give the image to OpenGL
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_BGR, GL_UNSIGNED_BYTE, data);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	printf("Loaded bmp.\n");
	return textureID;
}

GLuint ResLoader::compileShader(const char *shaderPath, GLenum shaderType) {
	uint8_t *shaderCode;
	size_t shaderCodeSize = bufferFile(shaderPath, &shaderCode);
	if (shaderCodeSize == -1) { //On error, no buffer is allocated.
		logger.error("Could not open file \"%s\"", shaderPath);
		return 0;
	} else if (shaderCodeSize == -2) {
		logger.error("File \"%s\" is too big to process", shaderPath);
		return 0;
	}

	GLint result = 0;
	int infoLogLength;

	//Create & compile the vertex shader.
	GLuint shaderID = glCreateShader(shaderType);
	glShaderSource(shaderID, 1, (const char**)&shaderCode, NULL);
	free(shaderCode); //GL copies the buffer.

	glCompileShader(shaderID);

	//Check the shader
	glGetShaderiv(shaderID, GL_COMPILE_STATUS, &result);
	if (result != GL_TRUE) {
		logger.error("Vertex shader compilation failed.");
	}

	//Print any diagnostics we have been given
	glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &infoLogLength);
	if (infoLogLength != 0) {
		char *shaderLog = (char*)malloc(infoLogLength);
		glGetShaderInfoLog(shaderID, infoLogLength, NULL, shaderLog);

		//Raise severity if compilation failed
		if (result != GL_TRUE) {
			logger.error("Shader Compilation Log:\n%s\n", shaderLog);
		} else {
			if (strcmp(shaderLog, "No errors.\n") != 0)
				logger.info("Shader Compilation Log:\n%s\n", shaderLog);
		}

		free(shaderLog);
	}

	if (result != GL_TRUE) {
		//If compilation failed, we still have the handle. Release it.
		glDeleteShader(shaderID);
		return 0;
	} else {
		return shaderID;
	}
}

ShaderHandle ResLoader::linkShaders(std::vector<GLuint> shaders) {
	GLuint programID = glCreateProgram();

	for (auto si = shaders.begin(), se = shaders.end(); si != se; ++si) {
		GLuint shader = *si;
		glAttachShader(programID, shader);
	}

	glLinkProgram(programID);

	GLint result;
	int infoLogLength;

	glGetProgramiv(programID, GL_LINK_STATUS, &result);
	if (result != GL_TRUE) {
		logger.error("Shader link failed.");
	}

	//Print any diagnostics we have been given
	glGetProgramiv(programID, GL_INFO_LOG_LENGTH, &infoLogLength);
	if (infoLogLength != 0) {
		char *linkerLog = (char*)malloc(infoLogLength);
		glGetProgramInfoLog(programID, infoLogLength, NULL, linkerLog);

		//Raise severity if link failed
		if (result != GL_TRUE) {
			logger.error("Shader Link Log:\n%s\n", linkerLog);
		} else {
			if (strcmp(linkerLog, "No errors.\n") != 0)
				logger.info("Shader Link Log:\n%s\n", linkerLog);
		}

		free(linkerLog);
	}

	for (auto si = shaders.begin(), se = shaders.end(); si != se; ++si) {
		GLuint shader = *si;
		glDeleteShader(shader);
	}

	if (result != GL_TRUE) {
		glDeleteProgram(programID);
		return nullptr;
	} else {
		return encodeHandle<ShaderHandle>(programID);
	}
}

ShaderHandle ResLoader::loadShaders(std::vector<std::pair<const char *, GLenum>> shaders) {
	std::vector<GLuint> shaderIDs;

	for (auto pi = shaders.begin(), pe = shaders.end(); pi != pe; ++pi) {
		const std::pair<const char*, GLenum> &shader = *pi;

		GLuint shaderID = compileShader(shader.first, shader.second);

		if (shaderID == 0) {
			//Free any shaders we had.
			for (auto si = shaderIDs.begin(), se = shaderIDs.end(); si != se; ++si) {
				GLuint id = *si;
				glDeleteShader(id);
			}
			return 0;
		}

		shaderIDs.push_back(shaderID);
	}

	return linkShaders(shaderIDs);
}
