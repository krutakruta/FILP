#include <iostream>
#include <math.h>
#include <memory.h>
#include "LinkedList.h"

using namespace std;

static const int level_count = 26;
static const int size_of_buffer = 33554432;

class BuddyStruct
{
	char* memory_start =reinterpret_cast<char*>(malloc(size_of_buffer));
	LinkedList<char*>* levels[level_count];
	int block_size[level_count];

	double const log2 = log(2);

public:
	BuddyStruct()
	{
		int tmp = 1;
		for (int i = 0; i < level_count; i++)
		{
			block_size[i] = tmp;
			tmp *= 2;
			levels[i] = new LinkedList<char*>();
		}
		levels[level_count - 1]->add(memory_start);
	}

	char* alloc(size_t size)
	{
		int level = get_level(size);
		if (levels[level]->size > 0)
		{
			//if we found good block
			char* ptr = levels[level]->first->item;
			levels[level]->pop_first();
			return ptr;
		}

		int g_level = level + 1;
		for (g_level; g_level < level_count; g_level++)
		{
			if (levels[g_level]->size > 0)
			{
				//found a block
				break;
			}
		}

		if (g_level == level_count)
		{
			//dont found a block
			throw "Out of memory";
		}

		char* ptr = levels[g_level]->first->item;
		levels[g_level]->pop_first();
		g_level--;

		for (g_level; g_level >= level; g_level--)
		{
			levels[g_level]->add(ptr + block_size[g_level]);
		}
		return ptr;
	}

	void dealloc(char* ptr, size_t size)
	{
		int level = get_level(size);
		_free(ptr, level);
	}

	~BuddyStruct()
	{
		free(reinterpret_cast<void*>(memory_start));
		delete levels;
	}

private:
	void _free(char* ptr, int level)
	{
		int buddy_number = (ptr - memory_start) / block_size[level];
		char* another_buddy;
		if (buddy_number % 2 == 0)
		{
			another_buddy = ptr + block_size[level];
		}
		else
		{
			another_buddy = ptr - block_size[level];
		}

		if (levels[level]->contains(another_buddy))
		{
			//find a buddy, union and going next level
			if (buddy_number % 2 == 1)
			{
				ptr = another_buddy;
			}
			levels[level]->pop(another_buddy);
			_free(ptr, level + 1);
		}
		else
		{
			//dont find a buddy, just add to the free list
			levels[level]->add(ptr);
		}
	}

	int get_level(int size)
	{
		return ceil(log(size) / log2);
	}
};

template<class T>
class BuddyAlloc
{
	int _size = sizeof(T);
public:
	using size_type = size_t;
	using difference_type = ptrdiff_t;
	using value_type = T;

	shared_ptr<BuddyStruct> alloc_str = nullptr;

	BuddyAlloc()
	{
		if (alloc_str == nullptr)
		{
			alloc_str = shared_ptr<BuddyStruct>(new BuddyStruct());
		}
	}

	template<class U>
	BuddyAlloc(BuddyAlloc<U> other)
	{
		alloc_str = other.alloc_str;
	}

	BuddyAlloc& operator=(const BuddyAlloc& ohter) = default;

	~BuddyAlloc() = default;

	void destroy(T* ptr)
	{
		((T*)ptr)->~T();
	}

	T* allocate(size_type n)
	{
		return reinterpret_cast<T*>(alloc_str->alloc(n * _size));
	}

	void deallocate(T* ptr, size_type n)
	{
		alloc_str->dealloc(reinterpret_cast<char*>(ptr), n * _size);
	}

	size_type max_size() const
	{
		return size_of_buffer / _size;
	}
};