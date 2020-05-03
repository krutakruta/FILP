#pragma once

using namespace std;

const size_t mb = 1024 * 1024;

template<typename F>
struct linkedListNode
{
	linkedListNode<F>* next;
	F* data;

	linkedListNode(F* elem)
	{
		next = nullptr;
		data = elem;
	}

	~linkedListNode()
	{
		delete data;
	}
};

template <typename T>
struct linkedList
{
	int ptr_count = 0;
	linkedListNode<T>* first;
	linkedListNode<T>* last;
	
	linkedList()
	{
		first = nullptr;
		last = nullptr;
		ptr_count = 1;
	}

	~linkedList()
	{
		linkedListNode<T>* current = first;
		while (current != nullptr)
		{
			linkedListNode<T>* next = current->next;
			delete current;
			current = next;
		}
	}

	void add(T* a)
	{
		linkedListNode<T>* node = new linkedListNode<T>(a);
		if (first == nullptr)
		{
			first = node;
			last = node;
		}
		else
		{
			last->next = node;
			last = node;
		}
	}
};

struct buffer
{
	size_t size;
	size_t ptr;
	char* data = nullptr;
	buffer(size_t s = mb)
	{
		size = s;
		ptr = 0;
		data = new char[size];
	}

	~buffer()
	{
		if (data != nullptr)
		{
			delete[] data;
		}
	}
};

template<class M>
struct mallocator
{
	using size_type = size_t;
	using difference_type = ptrdiff_t;
	using value_type = M;

	linkedList< buffer >* buffer_list;

	mallocator()
	{
		buffer_list = new linkedList<buffer>();
	}

	template<class U>
	mallocator(mallocator<U>& other) {
		//cout << "copyng" << endl;
		buffer_list = other.buffer_list;
		buffer_list->ptr_count++;
	}

	~mallocator()
	{
		//cout << "delet alloc" << endl;
		buffer_list->ptr_count--;
		if (buffer_list->ptr_count == 0)
		{
			delete buffer_list;
		}
	}

	M* allocate(size_type count)
	{
		size_t size_bytes = count * sizeof(M);
		linkedListNode<buffer>* current = buffer_list->first;
		while (current != nullptr)
		{
			if (current->data->size >= size_bytes)
			{
				//allocate
				char* ptr = (current->data->data + current->data->ptr);
				current->data->ptr += size_bytes;
				current->data->size -= size_bytes;
				return reinterpret_cast<M*>(ptr);
			}
			//cant allocate in this buffer
			current = current->next;
		}
		//cant find buffer to allocate
		buffer_list->add(new buffer());
		char* ptr = buffer_list->last->data->data;
		buffer_list->last->data->ptr += size_bytes;
		buffer_list->last->data->size -= size_bytes;
		return reinterpret_cast<M*>(ptr);
	}

	void deallocate(M* p, size_type n) {};

	size_type max_size() const {
		return mb / sizeof(value_type);
	}
};