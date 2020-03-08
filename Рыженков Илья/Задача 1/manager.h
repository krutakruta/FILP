#pragma once
#include <iostream>

using namespace std;

struct bufer {
	bufer* next = nullptr;
	void* data;
};

struct linkedList {
	int ptr_count = 0;
	const size_t bufer_size = 1048576;
	bufer* first = nullptr;
	bufer* last = nullptr;

	void add_buffer()
	{
		if (first == nullptr)
		{
			first = reinterpret_cast<bufer*>(malloc(bufer_size));
			first->data = reinterpret_cast<char*>(first) + sizeof(bufer*);
			first->next = nullptr;
			last = first;
			//cout << "Created first buffer" << endl;
			return;
		}
		bufer* next = reinterpret_cast<bufer*>(malloc(bufer_size));
		next->next = nullptr;
		last->next = next;
		last = next;
		last->data = reinterpret_cast<char*>(last) + sizeof(bufer*);
		//cout << "Created buffer" << endl;
	}

	void free_buffers()
	{
		bufer* current = first;
		while (current != nullptr)
		{
			bufer* next = current->next;
			free(current);
			current = next;
		}
	}

	size_t get_rest_size(bufer* buf)
	{
		return reinterpret_cast<char*>(buf) + bufer_size - reinterpret_cast<char*>(buf->data);
	}
};

template <typename T>
struct memoryManager {
	using value_type = T;
	using pointer = T * ;
	using const_pointer = const T*;
	using reference = T & ;
	using const_reference = const T&;
	using size_type = size_t;

	template< class U >
	struct rebind {
		typedef memoryManager<U> other;
	};

	linkedList* buffers;
	memoryManager() noexcept
	{
		//cout << "created new allocator at " << int(this) << endl;
		buffers = new linkedList;
		buffers->ptr_count++;
	}

	template <typename U>
	memoryManager(const memoryManager<U>& other)
	{
		//cout << "coping alloc 1 method" << int(this) << endl;
		buffers = other.buffers;
		buffers->ptr_count++;
	}

	memoryManager(const memoryManager& other)
	{
		//cout << "coping alloc 2 method" << int(this) << endl;
		buffers = other.buffers;
		buffers->ptr_count++;
	}

	//memoryManager& operator=(const memoryManager& x) = default;

	~memoryManager()
	{
		//cout << "delete alloc at " << int(this) << endl;
		buffers->ptr_count--;
		if (buffers->ptr_count == 0)
		{
			//cout << "deleting bufers" << endl;
			buffers->free_buffers();
		}
	}

	T* allocate(size_type size)
	{
		size = size * sizeof(T);
		bufer* current = buffers->first;
		int i = 0;
	
		while (current != nullptr)
		{
			if (buffers->get_rest_size(current) >= size)
			{
				void* ptr = current->data;
				current->data = reinterpret_cast<char*>(current->data) + size;
				//cout << "allocated " << size << " bytes in " << i << " buffer " << int(this) << " alloc" << endl;
				T* kek = reinterpret_cast<T*>(ptr);
				return kek;
			}
			current = current->next;
			i++;
		}
		buffers->add_buffer();
		void* ptr = buffers->last->data;
		buffers->last->data = reinterpret_cast<char*>(buffers->last->data) + size;
		cout << "allocated " << size << " bytes in created buffer " << int(this) << " alloc" << endl;
		T* kek = reinterpret_cast<T*>(ptr);
		return kek;
	}
	void deallocate(T* p, size_type n) {}
	size_type max_size() const
	{
		return buffers->bufer_size / sizeof(value_type);
	}
};