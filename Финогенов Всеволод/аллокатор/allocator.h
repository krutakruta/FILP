#pragma once
#include <iostream>
using namespace std;

#define BUF_SIZE 1024 * 1024

template <class T>
struct Buffer {
	uint8_t* memory;
	size_t offset;
	Buffer<T>* next = nullptr;
};


template <class T>
class CustomAllocator
{
public:
	Buffer<T>* mainBuffer;
	int* ref_count;
	typedef ptrdiff_t difference_type;
	typedef T* pointer;
	typedef const T* const_pointer;
	typedef T& reference;
	typedef const T& const_reference;
	typedef T value_type;

	CustomAllocator()
	{
		mainBuffer = CreateNewBuffer();
		ref_count = static_cast<int*>(malloc(sizeof(int)));
		*ref_count = 1;
		//cout << "created " << this << ", refs: " << *ref_count << endl;
	}

	template <class U>
	CustomAllocator(const CustomAllocator<U>& other)
	{
		mainBuffer = (Buffer<T>*)other.mainBuffer;
		ref_count = other.ref_count;
		(*ref_count)++;
		//cout << "created " << this << ", refs: " << *ref_count << endl;
	}

	CustomAllocator(const CustomAllocator& other) 
	{
		mainBuffer = (Buffer<T>*)other.mainBuffer;
		ref_count = other.ref_count;
		(*ref_count)++;
		//cout << "created " << this << ", refs: " << *ref_count << endl;
	}

	~CustomAllocator() {
		(*ref_count)--;
		//cout << "deleted " << this << ", refs: " << *ref_count << endl;
		if (*ref_count <= 0) {
			free(ref_count);
			FreeBuffer(mainBuffer);
		}	
	}

	void FreeBuffer(Buffer<T>* buffToFree) {
		if (!buffToFree)
			return;
		if (buffToFree->next)
			FreeBuffer(buffToFree->next);
		free(buffToFree);
	}

	Buffer<T>* CreateNewBuffer() {
		void* memory = malloc(sizeof(Buffer<T>) + BUF_SIZE);
		if (!memory)
			throw bad_alloc();
		Buffer<T>* buf = static_cast<Buffer<T>*>(memory);
		buf->memory = static_cast<uint8_t*>(memory) + sizeof(Buffer<T>);
		buf->next = nullptr;
		buf->offset = 0;
		return buf;
	}


	pointer allocate(size_t n) {
		//cout << "allocation from " << this << endl;
		if (!mainBuffer) {
			mainBuffer = CreateNewBuffer();
		}
		size_t size = sizeof(T) * n;
		if (size > BUF_SIZE)
			throw bad_alloc();
		Buffer<T>* currentBuffer = mainBuffer;
		while (true)
		{
			if (currentBuffer->offset + size > BUF_SIZE) {
				if (!currentBuffer->next)
					currentBuffer->next = CreateNewBuffer();
				currentBuffer = currentBuffer->next;
				continue;
			}
			pointer ptr = reinterpret_cast<pointer>(currentBuffer->memory + currentBuffer->offset);
			currentBuffer->offset += size;
			return ptr;
		}
	}

	void deallocate(void* p, size_t) {
		//cout << "start delocate" << endl;
	}

	pointer address(reference x) const { return &x; }
	const_pointer address(const_reference x) const { return &x; }
	CustomAllocator<T>& operator=(const CustomAllocator&) { return *this; }
	void construct(pointer p, const T& val)
	{
		new ((T*)p) T(val);
	}
	void destroy(pointer p) { p->~T(); }

	size_t max_size() const { return size_t(-1); }

	template <class U>
	struct rebind { typedef CustomAllocator<U> other; };



	template <class U>
	CustomAllocator& operator=(const CustomAllocator<U>&) { return *this; }
};

