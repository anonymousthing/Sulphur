#pragma once

#include <stdlib.h>
#include <stdio.h>

//Used instead of unique_ptr so we can move to different allocation strategies later.
template<typename T, size_t length = 0>
struct Buffer {
	T *data;

	Buffer() {
		data = (T*)malloc(sizeof(T) * length);
	}
	~Buffer() {
		free(data);
	}

	operator T*() {
		return data;
	}
};

//Used instead of unique_ptr so we can move to different allocation strategies later.
template<typename T>
struct Buffer<T, 0> {
	T *data;

	Buffer(size_t length) {
		data = (T*)malloc(sizeof(T) * length);
	}
	~Buffer() {
		free(data);
	}

	operator T*() {
		return data;
	}
};

struct FileHandle {
	FILE *inner;

	FileHandle(FILE *inner) : inner(inner) { }
	~FileHandle() {
		if (inner) {
			fclose(inner);
		}
	}

	operator FILE*() {
		return inner;
	}
};

//2 Dimensional array helper class.
template<typename T, size_t Width = 0, size_t Height = 0>
struct Array2D {
	T *data;

	Array2D() {
		data = (T*)malloc(sizeof(T) * Width * Height);
	}
	~Array2D() {
		free(data);
	}

	size_t size() {
		return sizeof(T) * Width * Height;
	}

	T &at(size_t x, size_t y) {
		return data[y * Width + x];
	}
	const T &at(size_t x, size_t y) const {
		return data[y * Width + x];
	}

	operator T*() {
		return data;
	}
};

//2 Dimensional array helper class.
template<typename T>
struct Array2D<T, 0, 0> {
	T *data;
	size_t width, height;

	Array2D(size_t width, size_t height)
		: width(width), height(height) {
		data = (T*)malloc(sizeof(T) * width * height);
	}
	~Array2D() {
		free(data);
	}

	size_t size() {
		return sizeof(T) * width * height;
	}

	T &at(size_t x, size_t y) {
		return data[y * width + x];
	}
	const T &at(size_t x, size_t y) const {
		return data[y * width + x];
	}

	operator T*() {
		return data;
	}
};

//3 Dimensional array helper class.
template<typename T, size_t Width = 0, size_t Height = 0, size_t Depth = 0>
struct Array3D {
	T *data;

	Array3D() {
		data = (T*)malloc(sizeof(T) * Width * Height * Depth);
	}
	~Array3D() {
		free(data);
	}

	size_t size() {
		return sizeof(T) * Width * Height * Depth;
	}

	T &at(size_t x, size_t y, size_t z) {
		return data[z * Width * Height + y * Width + x];
	}
	const T &at(size_t x, size_t y, size_t z) const {
		return data[z * Width * Height + y * Width + x];
	}

	operator T*() {
		return data;
	}
};

//3 Dimensional array helper class.
template<typename T>
struct Array3D<T, 0, 0, 0> {
	T *data;
	size_t width, height, depth;

	Array3D(size_t width, size_t height, size_t depth)
		: width(width), height(height), depth(depth) {
		data = (T*)malloc(sizeof(T) * width * height * depth);
	}
	~Array3D() {
		free(data);
	}

	size_t size() {
		return sizeof(T) * width * height * depth;
	}

	T &at(size_t x, size_t y, size_t z) {
		return data[z * width * height + y * width + x];
	}
	const T &at(size_t x, size_t y, size_t z) const {
		return data[z * width * height + y * width + x];
	}

	operator T*() {
		return data;
	}
};
