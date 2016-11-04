#pragma once

//ThreadSafe Deque
#include <atomic>
#include <mutex>

template<typename T, size_t defaultCapacity = 16, size_t smallestSize = 4>
class TSDeque {
	T *start;	//The beginning of allocated memory.
	T *end;		//1 byte beyond the last allocated byte in memory (The address of the first 'invalid' element)
	T *head;	//The address of the first element.
	T *tail;	//The address after the last element.

	//If head == end, the queue is empty.
	//If head < tail, there is 1 continuous block of items.
	//If head > tail, there are 2 regions of T. head -> end and start -> tail
	//If head == tail, the queue is full.
	
	mutable std::mutex lock;
	
	//Assumes newCapacity > oldCapacity
	//Valid for empty queues.
	void expand(size_t newCapacity, bool freeOld = true) {
		//Allocate new queue
		size_t allocSize = sizeof(T) * newCapacity;
		T *newStart = (T*)malloc(allocSize);

		if (head == end) {
			//Empty queue
			tail = head = newStart;
		} else if (head < tail) {
			//head -> tail in 1 single run.
			T *oldCurrent = head;
			T *newCurrent = newStart;

			while (oldCurrent < tail) {
				*newCurrent = std::move(*oldCurrent);
				++oldCurrent; ++newCurrent;
			}

			head = newStart;
			tail = newCurrent;
		} else {
			//tail wraps around
			//head -> end, start -> tail

			//Move head -> end
			T *oldCurrent = head;
			T *newCurrent = newStart;

			while (oldCurrent < end) {
				*newCurrent = std::move(*oldCurrent);
				++oldCurrent; ++newCurrent;
			}

			//Move start -> tail
			oldCurrent = start;
			while (oldCurrent < tail) {
				*newCurrent = std::move(*oldCurrent);
				++oldCurrent; ++newCurrent;
			}

			head = newStart;
			tail = newCurrent;
		}

		//Free old queue
		if (freeOld)
			free(start);

		start = newStart;
		end = start + newCapacity;
	}

	void expand() {
		if (head == end) {
			expand(smallestSize);
		} else {
			size_t currentSize = end - start - 1;
			expand(currentSize * 2);
		}
	}

	void copy(const TSDeque &other) {
		//We cheat here.
		//We set our pointers to other, then call "expand", ensuring it doesn't free the old pointers.
		other.lock.lock();

		start = other.start;
		end = other.end;
		head = other.head;
		tail = other.tail;

		if (head == end) {
			expand(smallestSize);
		} else {
			size_t currentSize = end - start - 1;
			expand(currentSize, false);
		}

		other.lock.unlock();
	}

public:
	TSDeque() {
		static_assert(defaultCapacity >= 0, "TSQueue cannot have a negative default capacity.");
		static_assert(smallestSize >= 1, "The smallest size a TSQueue can be allowed to be is 1.");

		if (defaultCapacity == 0) {
			start = end = head = tail = nullptr;
		} else {
			size_t allocSize = sizeof(T) * defaultCapacity;

			start = tail = (T*)malloc(allocSize);
			head = end = start + defaultCapacity;
		}
	}

	TSDeque(const TSDeque &original) {
		copy(other);
	}

	TSDeque(TSDeque &&original) : lock(std::move(original.lock)) {
		start = original.start;
		end = original.end;
		head = original.head;
		tail = original.tail;
	}

	inline TSDeque &operator=(TSDeque &&other) {
		this->~TSDeque();

		start = original.start;
		end = original.end;
		head = original.head;
		tail = original.tail;
		return *this;
	}

	inline TSDeque &operator=(const TSDeque &other) {
		this->~TSDeque();

		copy(other);
		
		return *this;
	}

	~TSDeque() {
		if (head == end) {
			//Empty queue
			//Nothing to destruct.
		} else if (head < tail) {
			//head -> tail in 1 single run.
			T *current = head;
			while (current < tail) {
				current->~T();
			}
		} else {
			//tail wraps around (head -> end, start -> tail)

			//Destruct head -> end
			T *current = head;
			while (current < end) {
				current->~T();
			}

			//Destruct start -> tail
			current = start;
			while (current < tail) {
				current->~T();
			}
		}

		free(start);
	}

	inline size_t size() const {
		lock.lock();

		size_t size;
		if (head == end) {
			size = 0;
		} else if(head < tail) {
			size = tail - head;
		} else {
			size = (end - head) + (tail - start);
		}

		lock.unlock();
		return size;
	}

	inline size_t capacity() const {
		lock.lock();

		size_t capacity = end - start - 1;

		lock.unlock();

		return capacity;
	}

	//Easy names
	inline void push(const T &item) {
		push_back(item);
	}

	inline bool tryPop(T &item) {
		return tryPop_front(item);
	}

	//Standard names provided for compat with standard collections (drop in)

	void push_back(const T &item) {
		lock.lock();

		if (head == end) {
			//Empty.
			//Check to see we have space.
			if (!start) {
				expand();
			}

			tail = head = start;
		} else if (head == tail) {
			//Full
			expand();
		}

		*tail = item;
		++tail;
		if (tail >= end) {
			tail = start;
		}

		lock.unlock();
	}

	void push_front(const T &item) {
		lock.lock();

		if (head == end) {
			//Empty.
			//Check to see we have space.
			if (!start) {
				expand();
			}

			tail = head = start;
		} else if (head == tail) {
			//Full
			expand();
		}

		//Move the head back one
		--head;
		if (head < start) {
			head = end - 1;
		}

		*head = item;

		lock.unlock();
	}

	bool tryPop_front(T &item) {
		lock.lock();

		if (head == end) {
			//Deque is empty.
			lock.unlock();
			return false;
		}

		item = std::move(*head);
		++head;
		if (head == end && tail != start)
			head = start;

		if (head == tail) {
			head = end;
		}

		lock.unlock();
		return true;
	}

	bool tryPop_back(T &item) {
		lock.lock();

		if (head == end) {
			//Deque is empty.
			lock.unlock();
			return false;
		}

		//Tail points to one beyond the end.
		if (tail == start) {
			tail = end - 1;
		} else {
			--tail;
		}
		item = std::move(*tail);

		//If we've just moved tail on top of head, then we've just emptied the deque.
		if (head == tail) {
			head = end;
		}

		lock.unlock();
		return true;
	}
};
