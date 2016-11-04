#pragma once

#include <stdlib.h>
#include <assert.h>

//Used because we don't know how much memory we need.
template <typename T>
struct LinkedBuffer {
	T *data;
	size_t size;
	size_t position;
	LinkedBuffer *next;

	//Size in floats.
	LinkedBuffer(size_t size)
		: size(size), position(0), next(nullptr) {
		data = (T*)malloc(size * sizeof(T));
	}

	~LinkedBuffer() {
		free(data);
		data = nullptr;

		delete next;
		next = nullptr;
	}

	//Fills in the 'next' pointer and returns the new LinkedBuffer
	LinkedBuffer *expand() {
		//Seek to end.
		LinkedBuffer *end = this;
		while (end->next) {
			end = end->next;
		}

		LinkedBuffer *buff = new LinkedBuffer(size);
		end->next = buff;
		return buff;
	}

	//Expands the buffer to ensure that count places are available.
	LinkedBuffer *accommodate(size_t count) {
		if (count > getRemaining()) {
			return expand();
		} else {
			return this;
		}
	}

	//Moves the buffer forward by the given number of elements.
	void advance(size_t amount) {
		assert(getRemaining() >= amount);
		position += amount;
	}

	//Steals the data from a given linked buffer.
	T *take() {
		T *ptr = data;
		data = nullptr;
		size = position = 0;
		return ptr;
	}

	//Returns a consolidated array of all data from the buffer and any linked buffers.
	T *takeConsolidated() {
		if (next == nullptr) {
			return take();
		} else {
			size_t count = 0;
			LinkedBuffer *buff = this;
			while (buff) {
				count += buff->position;
				buff = buff->next;
			}

			T *consolidated = (T*)malloc(sizeof(T) * count);

			size_t dataSoFar = 0;
			buff = this;
			while (buff) {
				memcpy(buff + (dataSoFar * 3), buff->data, buff->size * sizeof(float));
				dataSoFar += buff->size;

				buff = buff->next;
			}

			//Remove our data (the method says 'take')
			free(data);
			data = nullptr;
			size = position = 0;

			return consolidated;
		}
	}

	//The number of additional floats we can store.
	size_t getRemaining() const {
		return size - position;
	}

	operator T*() {
		return &data[position];
	}
};

