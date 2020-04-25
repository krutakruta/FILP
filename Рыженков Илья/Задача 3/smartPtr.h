#pragma once

using namespace std;

template<class T>
class smartPtr
{
	T* ptr = nullptr;
	int* ref_count = nullptr;

public:
	void Release()
	{
		if (ptr == nullptr)
		{
			return;
		}
		*ref_count = *ref_count - 1;
		if (*ref_count == 0)
		{
			delete ptr;
			delete ref_count;
		}
		ptr = nullptr;
		ref_count = nullptr;
	}

	T Get()
	{
		return *ptr;
	}

	void Set(T* obj_ptr)
	{
		Release();
		ptr = obj_ptr;
		ref_count = new int[1];
		*ref_count = 1;
	}

	smartPtr(T* obj_ptr)
	{
		Set(obj_ptr);
	}

	smartPtr(const smartPtr& x)
	{
		ptr = x.ptr;
		ref_count = x.ref_count;
		if (ptr != nullptr)
		{
			*ref_count = *ref_count + 1;
		}
	}

	smartPtr()
	{
		ptr = nullptr;
		ref_count = nullptr;
	}

	smartPtr<T>& operator=(smartPtr<T>& smart_ptr)
	{
		Release();
		ptr = smart_ptr.ptr;
		ref_count = smart_ptr.ref_count;
		*ref_count = *ref_count + 1;
		return *this;
	}

	T* operator->()
	{
		return ptr;
	}

	T operator*()
	{
		return *ptr;
	}

	~smartPtr()
	{
		Release();
	}
};
