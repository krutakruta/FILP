#include <iostream>

using namespace std;

template <class T>
class SmartPtr {
    T* Ptr;
    size_t* RefCount;

    void Attach(const SmartPtr& Other) {
        if (Other.Ptr) {
            Ptr = Other.Ptr;
            RefCount = Other.RefCount;
            (*RefCount)++;
        }
    }

public:
    SmartPtr() {
        Ptr = nullptr;
        RefCount = nullptr;
    }

    ~SmartPtr() {
        Release();
    }

    SmartPtr(const SmartPtr& Other) {
        Attach(Other);
    }

    SmartPtr& operator=(const SmartPtr& Other) {
        if (this == &Other) {
            return *this;
        }
        Release();
        Attach(Other);
        return *this;
    }

    T* operator->() {
        return Ptr;
    }

    T& operator*() {
        return *Ptr;
    }

    void Set(T* ObjPtr) {
        Ptr = ObjPtr;
        RefCount = new size_t;
        *RefCount = 1;
    }

    T* Get()
    {
        return Ptr;
    }

    void Release() {
        if (Ptr) {
            (*RefCount)--;
            if ((*RefCount) == 0) {
                delete Ptr;
                delete RefCount;
            }
            Ptr = nullptr;
            RefCount = nullptr;
        }
    }
};

int main()
{
    SmartPtr<int> SmartPtr1 = SmartPtr<int>();
    cout << SmartPtr1.Get() << endl;
    SmartPtr1.Set(new int(7));
    cout << SmartPtr1.Get() << endl;
    SmartPtr1.Release();
    cout << SmartPtr1.Get() << endl;
    SmartPtr1.Set(new int(22));

    SmartPtr<int> SmartPtr2 = SmartPtr<int>();
    SmartPtr2 = SmartPtr1;
    cout << SmartPtr2.Get() << endl;
    SmartPtr1.Release();
    cout << SmartPtr1.Get() << endl;
    cout << SmartPtr2.Get() << endl;
}
