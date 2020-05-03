#include<iostream>

using namespace std;

class TestClass
{
public:
	int value;

	TestClass(int a)
	{
		value = a;
	}
};

class ReferenceCounter
{
private:
	size_t count;

public:
	ReferenceCounter()
	{
		count = 0;
	}

	void AddRef()
	{
		count++;
	}

	size_t ReleaseRef()
	{
		return --count;
	}

	size_t GetCount()
	{
		return count;
	}
};

template<class T>
class SmartPointer
{
private:
	T* ptr;
	ReferenceCounter* ref_count;

	void remove()
	{
		if (ref_count->ReleaseRef() == 0)
		{
			delete ptr;
			delete ref_count;
			printf("All smart pointers, ref count and object have been destroyed\n");
		}
		else
		{
			printf("One smart pointer has been destroyed. Pointer: %p, Ref count: %d\n", ptr, ref_count->GetCount());
		}
	}

public:
	SmartPointer()
	{
		ref_count = new ReferenceCounter();
		ref_count->AddRef();
	}

	SmartPointer(T* object)
	{
		ptr = object;
		ref_count = new ReferenceCounter();
		ref_count->AddRef();
		printf("Object with smart pointer have been created. Pointer: %p, Ref count: %d\n", ptr, ref_count->GetCount());
	}

	SmartPointer(const SmartPointer<T>& other)
	{
		ptr = other.ptr;
		ref_count = other.ref_count;
		ref_count->AddRef();
		printf("Smart pointer has been copied. Pointer: %p, Ref count: %d\n", ptr, ref_count->GetCount());
	}

	~SmartPointer()
	{
		remove();
	}

	T& operator*()
	{
		return *ptr;
	}

	T* operator->()
	{
		return ptr;
	}

	SmartPointer<T>& operator=(SmartPointer<T>& other)
	{
		if (this != &other)
		{
			if (ref_count->ReleaseRef() == 0)
			{
				delete ptr;
				delete ref_count;
			}

			ptr = other.ptr;
			ref_count = other.ref_count;
			ref_count->AddRef();
		}
		return *this;
	}

	void Release()
	{
		remove();
		ptr = nullptr;
		ref_count = new ReferenceCounter();
		ref_count->AddRef();
	}

	T Get()
	{
		return *ptr;
	}

	void Set(T* new_ptr)
	{
		if (ptr == nullptr)
		{
			ptr = new_ptr;
		}
		else
		{
			*ptr = *new_ptr;
		}
		printf("Object has been changed. Pointer: %p, Ref count: %d\n", ptr, ref_count->GetCount());
	}
};


void Test1()
{
	printf("Test1\n");
	TestClass* test_obj = new TestClass(1);
	SmartPointer<TestClass> ptr1(test_obj);
	SmartPointer<TestClass> ptr2 = ptr1;

	TestClass* test_obj2 = new TestClass(2);
	ptr1.Set(test_obj2);
	printf("Another pointer's object: %d\n", ptr2->value);
}

void Test2()
{
	printf("\nTest2\n");
	TestClass* test_obj = new TestClass(1);
	SmartPointer<TestClass> ptr1(test_obj);
	SmartPointer<TestClass> ptr2 = ptr1;

	ptr1.Release();
}

int main()
{
	Test1();
	Test2();

	return 0;
}

