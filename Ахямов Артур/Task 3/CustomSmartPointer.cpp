#pragma once
#include <iostream>


template<typename T>
class SmartPointer
{
public:
	T Get();
	void Set(T newObject);
	void Release();
	T& operator*();
	T* operator->();

	SmartPointer<T>& operator=(const SmartPointer<T>& another)
	{
		if (this == &another)
			return *this;
		Release();
		AllocateMemoryForPointerIfItsNull();
		objectPointer = another.objectPointer;
		referenceCounter = another.referenceCounter;
		++(*referenceCounter);
		return *this;
	}

	SmartPointer()
	{
		objectPointer = NULL;
		referenceCounter = new unsigned(0);
	}

	SmartPointer(T* objectPointer)
	{
		this->objectPointer = objectPointer;
		referenceCounter = new unsigned(1);
	}

	~SmartPointer()
	{
		Release();
	}
private:
	unsigned int* referenceCounter;
	T* objectPointer;
	void AllocateMemoryForPointerIfItsNull();
};

template<typename T>
void SmartPointer<T>::AllocateMemoryForPointerIfItsNull()
{
	if (objectPointer == NULL)
		objectPointer = (T*)malloc(sizeof(T));
}

template<typename T>
T SmartPointer<T>::Get()
{
	return *objectPointer;
}

template<typename T>
void SmartPointer<T>::Set(T object)
{
	AllocateMemoryForPointerIfItsNull();
	*objectPointer = object;
	*referenceCounter = 1;
}

template<typename T>
void SmartPointer<T>::Release()
{
	(*referenceCounter)--;
	if (*referenceCounter == 0)
	{
		if (objectPointer != NULL)
			delete objectPointer;
		delete referenceCounter;
	}
}

template<typename T>
T& SmartPointer<T>::operator*()
{
	return *objectPointer;
}

template<typename T>
T* SmartPointer<T>::operator->() 
{
	return objectPointer;
}

int main()
{
	auto pointer = new SmartPointer<std::string>(new std::string("11111"));
	// Get
	std::cout << "Get: " << pointer->Get() << "\n\n";
	// Set
	std::cout << "Previous value: " << pointer->Get() << "\n";
	pointer->Set("22222");
	std::cout << "Value after Set(22222): " << pointer->Get() << "\n";
	// Release
	pointer->Release();
	return 0;
}