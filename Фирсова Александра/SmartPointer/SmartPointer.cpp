#include <iostream>

using namespace std;

template <typename T>
class SmartPointer
{
private:
	T* m_obj;

public:
	int* use_count;

	SmartPointer() {
		m_obj = nullptr;
		use_count = nullptr;
	}

	~SmartPointer() {
		Release();
	}

	T* Get(){
		return m_obj;
	}

	void Set(T* obj){
		Release();
		m_obj = obj;
		use_count = new int(1);
	}

	void Release(){
		if (m_obj == nullptr) {
			return;
		}

		(*use_count)--;

		if (*use_count == 0) {
			delete m_obj;
			delete use_count;
		}

		m_obj = nullptr;
		use_count = nullptr;
	}

	   
	SmartPointer& operator=(SmartPointer & ptr) {
		Release();

		if (this != &ptr) {
			m_obj = ptr.Get();
			use_count = ptr.use_count;
			(*use_count)++;
		}
		return *this;
	}

	T* operator->(){
		return m_obj;
	}

	T& operator*(){
		return *m_obj;
	}
};


int main()
{
	SmartPointer<int> ptr1;
	SmartPointer<int> ptr2;
	
	cout << "setting value to ptr1\n";
	ptr1.Set(new int(2));
	cout << "ptr1 count after setting value: " << *ptr1.use_count << "\n\n";

	cout << "changing value in ptr1\n";
	ptr1.Set(new int(3));
	cout << "ptr1 count after changing value: " << *ptr1.use_count << "\n\n";

	cout << "assigning ptr1 to ptr2\n";
	ptr2 = ptr1;
	cout << "ptr2 count: " << *ptr2.use_count << '\n';
	cout << "tptr1 count: " << *ptr1.use_count << "\n\n";

	cout << "releasing ptr1\n";
	ptr1.Release();
	cout << "ptr2 count: " << *ptr2.use_count << '\n';
}