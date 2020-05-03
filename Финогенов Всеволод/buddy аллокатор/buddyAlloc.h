#pragma once
#include <iostream>
using namespace std;

#define MAX_SIZE 1024 * 1024 * 64
#define MIN_SIZE 32

template <class T>
struct Buddy {
	size_t size;
	int state; // 0 - free, 1 - split, 2 - occupied
	int splits;
	uint8_t* memory;
	Buddy<T>* parent = nullptr;
	Buddy<T>* left = nullptr;
	Buddy<T>* right = nullptr;
};

template <class T>
class BuddyAlloc
{
public:
	Buddy<T>* mainBuddy;
	int* ref_count;
	typedef ptrdiff_t difference_type;
	typedef T* pointer;
	typedef const T* const_pointer;
	typedef T& reference;
	typedef const T& const_reference;
	typedef T value_type;

	BuddyAlloc()
	{
		ref_count = static_cast<int*>(malloc(sizeof(int)));
		*ref_count = 1;
		mainBuddy = CreateNewBuddy();
	}

	template <class U>
	BuddyAlloc(const BuddyAlloc<U>& other)
	{
		mainBuddy = (Buddy<T>*)other.mainBuddy;
		ref_count = other.ref_count;
		(*ref_count)++;
	}

	BuddyAlloc(const BuddyAlloc& other)
	{
		mainBuddy = (Buddy<T>*)other.mainBuddy;
		ref_count = other.ref_count;
		(*ref_count)++;
	}

	~BuddyAlloc() {
		(*ref_count)--;
		if (*ref_count <= 0) {
			free(ref_count);
			free(mainBuddy);
		}
	}

	Buddy<T>* SplitBuddy(Buddy<T>* parent) {
		size_t newSize = parent->size / 2;
		if (parent->splits <= 0 || parent->state != 0)
			throw bad_alloc();
		parent->state = 1;
		int newSplits = parent->splits / 2 - 1;
		Buddy<T>* left = reinterpret_cast<Buddy<T>*>(parent->memory);
		left->size = newSize;
		left->memory = parent->memory + sizeof(Buddy<T>);
		left->splits = newSplits;
		left->state = 0;
		left->parent = parent;
		uint8_t *rightPlace = left->memory + newSize + newSplits * sizeof(Buddy<T>);
		Buddy<T>* right = reinterpret_cast<Buddy<T>*>(rightPlace);
		right->size = newSize;
		right->memory = rightPlace + sizeof(Buddy<T>);
		right->splits = newSplits;
		right->state = 0;
		right->parent = parent;
		parent->left = left;
		parent->right = right;
		return left;
	}

    Buddy<T>* CreateNewBuddy() {
		int numOfSplits = (MAX_SIZE / MIN_SIZE) * 2 - 1;
		void* memory = malloc(MAX_SIZE + numOfSplits * sizeof(Buddy<T>));
		if (!memory)
			throw bad_alloc();
		mainBuddy = static_cast<Buddy<T>*>(memory);
		mainBuddy->splits = numOfSplits - 1;
		mainBuddy->state = 0;
		mainBuddy->memory = static_cast<uint8_t*>(memory) + sizeof(Buddy<T>);
		mainBuddy->size = MAX_SIZE;
		return mainBuddy;
	}

	pointer allocate(size_t n) {
		if (!mainBuddy) {
			mainBuddy = CreateNewBuddy();
		}
		size_t size = sizeof(T) * n;
		if (size > MAX_SIZE)
			throw bad_alloc();
		Buddy<T>* currentBuddy = GetBestFittingSubbuddy(mainBuddy, size);
		while (currentBuddy->size != MIN_SIZE && currentBuddy->size / 2 >= size) {
			currentBuddy = SplitBuddy(currentBuddy);
		}
		currentBuddy->state = 2;
		return reinterpret_cast<pointer>(currentBuddy->memory);
	}

	Buddy<T>* GetBestFittingSubbuddy(Buddy<T> *bud, size_t size) {
		if (size > MAX_SIZE)
			throw bad_alloc();
		if (bud->state == 2 || bud->size < size)
			return nullptr;
		if (bud->state == 0)
			return bud;
		Buddy<T>* left = GetBestFittingSubbuddy(bud->left, size);
		Buddy<T>* right = GetBestFittingSubbuddy(bud->right, size);
		if (!left && !right)
			return nullptr;
		if (!left)
			return right;
		if (!right)
			return left;
		if (right->size < left->size)
			return right;
		return left;
	}

	void deallocate(void* p, size_t) {
		return;
	}

	pointer address(reference x) const { return &x; }
	const_pointer address(const_reference x) const { return &x; }
	BuddyAlloc<T>& operator=(const BuddyAlloc&) { return *this; }
	void construct(pointer p, const T& val)
	{
		new ((T*)p) T(val);
	}
	void destroy(pointer p) { p->~T(); }

	size_t max_size() const { return size_t(-1); }

	template <class U>
	struct rebind { typedef BuddyAlloc<U> other; };



	template <class U>
	BuddyAlloc& operator=(const BuddyAlloc<U>&) { return *this; }
};

