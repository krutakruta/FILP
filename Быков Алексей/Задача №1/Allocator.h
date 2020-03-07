#pragma once
#include "list.h"
/* Allocator starts */
const size_t mbyte = 1024 * 1024; // One Megabyte as constant for alloc

class linked_buffer {
private:
	struct buffer_params {
		char* buffer = nullptr;
		size_t ptr;
		size_t size;
		buffer_params(size_t _size = mbyte) {
			buffer = new char[_size];
			ptr = 0;
			size = _size;
		}

		~buffer_params() {
			if (buffer != nullptr)
				delete[] buffer;
		}

		buffer_params(const buffer_params& x) = delete;
		buffer_params& operator=(const buffer_params& x) = delete;
	};
	LinkedList<buffer_params>* current;
public:
	linked_buffer(size_t size = mbyte) {
		current = new LinkedList<buffer_params>(new buffer_params());
	}

	~linked_buffer() {
		delete current;
	}

	linked_buffer(const linked_buffer& x) = delete;
	linked_buffer& operator=(const linked_buffer& x) = delete;

	void* alloc(size_t n, size_t type_size) {
		if (current->a->ptr + type_size * n > current->a->size / type_size)
			current = current->push_back(new buffer_params());
		void* _Ptr = current->a->buffer + current->a->ptr;
		current->a->ptr += type_size * n;
		return _Ptr;
	}
};

template<class T>
class MyAlloc {
public:
	using size_type = size_t;
	using difference_type = ptrdiff_t;

	using value_type = T;

	std::shared_ptr<linked_buffer> _Cur_Buffer = nullptr;

	MyAlloc() {
		if (_Cur_Buffer == nullptr)
			_Cur_Buffer = std::shared_ptr<linked_buffer>(new linked_buffer());
	}

	template<class U>
	MyAlloc(MyAlloc<U>& other) {
		_Cur_Buffer = other._Cur_Buffer;
	}

	MyAlloc& operator=(const MyAlloc& x) = default;

	~MyAlloc() = default;

	T* allocate(size_type n) {
		return reinterpret_cast<T*>(_Cur_Buffer->alloc(n, sizeof(T)));
	}

	void deallocate(T* p, size_type n) {
		/* * * */
	}

	void destroy(T* _Ptr) {
		((T*)_Ptr)->~T();
	}

	size_type max_size() const {
		return mbyte / sizeof(value_type);
	}
};

/* Allocator ends */