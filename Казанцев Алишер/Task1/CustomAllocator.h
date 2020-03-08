using namespace std;

class Buffer {
public:
	size_t size = 1024 * 1024;
	size_t ptr;
	char *buffer;
	Buffer *next;

	Buffer() {
		buffer = new char[size];
		ptr = 0;
		next = NULL;
	}

	~Buffer() {
		if (buffer != nullptr)
			delete[] buffer;
	}
};

class BufferList {
public:
	Buffer *current;
	Buffer *top;

	BufferList() {
		current = new Buffer();
		top = current;		
	}

	~BufferList() {
		while (top != NULL) {
			Buffer* deletable = top;
			top = top->next;
			delete deletable;
		}
	}

	void* allocate(size_t length, size_t size) {
		if (current->ptr + size * length > current->size) {
			current->next = new Buffer();
			current = current->next;
		}

		void* new_ptr = current->buffer + current->ptr;
		current->ptr += size * length;
		return new_ptr;
	}
};

template <class T> class CustomAllocator {
public:
	typedef T value_type;
	typedef T* pointer;
	typedef const T* const_pointer;
	typedef T& reference;
	typedef const T& const_reference;
	typedef size_t size_type;
	typedef ptrdiff_t difference_type;

	BufferList* buffers;

	CustomAllocator() {
		buffers = new BufferList();
	};

	template<class U>
	CustomAllocator(CustomAllocator<U>& other) {
		buffers = other.buffers;
	}

	pointer allocate(size_type size) {
		return reinterpret_cast<T*>(buffers->allocate(size, sizeof(T)));
	};

	void destroy(pointer p) {
		p->~T();
	};

	void deallocate(pointer p, size_type n) {};
};
