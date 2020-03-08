#include<iostream>
#include<Windows.h>
#include<tchar.h>
#include<map>
#include<string>
#include<time.h>
#include<algorithm>
#include<vector>

using namespace std;

const size_t MEGABYTE = 1024 * 1024;

bool map_sorting_comparator(pair<char*, size_t>, pair<char*, size_t>);

struct comparator
{
	bool operator()(char* first, char* second) const
	{
		while (isalpha((unsigned char)*first) && isalpha((unsigned char)*second))
		{
			if (tolower(*first) < tolower(*second))
				return true;
			if (tolower(*first) > tolower(*second))
				return false;
			first++;
			second++;
		}
		if (isalpha((unsigned char)*first) && !isalpha((unsigned char)*second))
			return true;
		return false;
	}
};

struct buffer
{
	size_t max_size;
	char* content = nullptr;
	size_t current_pos;
	buffer* next = nullptr;

	buffer(size_t size)
	{
		max_size = size;
		current_pos = 0;
		content = new char[size];
	}

	~buffer()
	{
		if (content != nullptr)
			delete[] content;
		if (next != nullptr)
			next->~buffer();
	}
};

struct linked_list
{
	buffer* head;
	buffer* tail;
	size_t _size = 0;

	linked_list()
	{
		_size = 0;
		head = nullptr;
		tail = nullptr;
	}

	~linked_list()
	{
		if (head != nullptr)
			head->~buffer();
	}

	void* alloc(size_t amount, size_t size)
	{
		if (_size == 0)
		{
			add_buffer();
			return alloc_existing_buffer(amount, size);
		}

		if (tail != nullptr && tail->current_pos + amount * size >= tail->max_size)
		{
			add_buffer();
			return alloc_existing_buffer(amount, size);
		}
		return alloc_existing_buffer(amount, size);
	}

	void* alloc_existing_buffer(size_t amount, size_t size)
	{
		void* result = tail->content + tail->current_pos;
		tail->current_pos += amount * size;
		return result;
	}

	void add_buffer()
	{
		buffer* new_buff = new buffer(MEGABYTE);
		if (_size == 0)
		{
			head = new_buff;
			tail = new_buff;
		}
		else
		{
			tail->next = new_buff;
		}
		_size++;
	}
};

template<typename T>
struct my_allocator
{
	typedef T value_type;
	typedef T* pointer;
	typedef const T* const_pointer;
	typedef T& reference;
	typedef const T& const_reference;
	typedef size_t size_type;
	typedef ptrdiff_t difference_type;

	linked_list* buffers = nullptr;

	my_allocator()
	{
		if (buffers == nullptr)
			buffers = new linked_list();	
	}

	template <typename U>
	my_allocator(my_allocator<U>& other)
	{
		buffers = other.buffers;
	}

	~my_allocator()
	{
	}

	T* allocate(size_t n)
	{
		return reinterpret_cast<T*>(buffers->alloc(n, sizeof(T)));
	}

	void deallocate(T* p, size_t n)
	{
	}
};

int main()
{
	char path[30] = "book.txt";
	HANDLE fileHandle = CreateFile(path, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (fileHandle == INVALID_HANDLE_VALUE)
	{
		printf("Reading file error");
		CloseHandle(fileHandle);
		return -1;
	}
	LARGE_INTEGER size;
	GetFileSizeEx(fileHandle, &size);
	char* text = new char[size.u.LowPart + 1];
	if (!ReadFile(fileHandle, text, size.u.LowPart, 0, NULL))
	{
		printf("Reading file error");
		CloseHandle(fileHandle);
		return -2;
	}
	CloseHandle(fileHandle);


	//My allocator
	map<char*, size_t, comparator, my_allocator< char*> > m;
	clock_t start_time = clock();
	bool word_started = false;
	for (size_t i = 0; i < size.u.LowPart; ++i)
	{
		if (text[i] == '\0')
			break;
		if (isalpha((unsigned char)text[i]))
		{
			if (!word_started)
			{
				m[text + i]++;
				word_started = true;
			}
		}
		else
		{
			word_started = false;
		}
	}
	double time = ((double)(clock() - start_time) / CLOCKS_PER_SEC);
	printf("My allocator time: %.3f sec\n", time);
	//My allocator end


	//Default allocator start
	map<string, size_t> m1;
	start_time = clock();
	string current_word = "";
	for (int i = 0; i < size.u.LowPart; ++i)
	{
		if (text[i] == '\0')
			break;
		if (!isalpha((unsigned char)text[i]))
		{
			if (current_word != "")
				m1[current_word]++;
			current_word = "";
		}
		else
		{
			current_word += tolower(text[i]);
		}
	}
	time = ((double)(clock() - start_time) / CLOCKS_PER_SEC);
	printf("Default allocator time: %.3f sec\n", time);
	//Default allocator end

	//Sorting words start
	vector< pair<char*, size_t> > v;
	for (map<char*, size_t, comparator, my_allocator< char*> >::iterator i = m.begin(); i != m.end(); ++i)
		v.push_back(make_pair(i->first, i->second));

	sort(v.begin(), v.end(), map_sorting_comparator);

	for (vector< pair<char*, size_t> >::iterator i = v.begin(); i != v.end(); ++i)
	{
		char* current = i->first;
		while (isalpha((unsigned char)current[0]))
		{
			printf("%c", tolower(current[0]));
			current++;
		}
		printf(" %d\n", i->second);
	}
	//Sorting word end

	return 0;
}

bool map_sorting_comparator(pair<char*, size_t> a, pair<char*, size_t> b)
{
	return a.second > b.second;
}