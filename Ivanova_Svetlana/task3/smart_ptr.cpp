#include <cstddef>
#include <stdio.h>
#include <iostream>

using namespace std;

template<class T>
class CSmartPtr {
    T *Ptr;
    size_t *Count;

    void Attach(const CSmartPtr &Other) {
        Ptr = Other.Ptr;
        Count = Other.Count;
        (*Count)++;
    }

public:
    void Set(T *APtr) {
        printf("set\n");
        Count = new size_t;
        Ptr = APtr;
        *Count = 1;
    }

    CSmartPtr() {
        printf("init\n");
        Count = NULL;
        Ptr = NULL;
    }

    T Get() {
        //printf("get\n");
        return *Ptr;
    }

    T *operator->() {
        printf("->\n");
        return Ptr;
    }

    T &operator*() {
        printf("*\n");
        return *Ptr;
    }

    void Release() {
        printf("release, count before: %d\n", (int) *Count);
        if (!Count)
            return;
        (*Count)--;
        if ((*Count) == 0) {
            printf("delete\n");
            delete Count;
            delete Ptr;
        }
        Count = NULL;
        Ptr = NULL;
    }

    ~CSmartPtr() {
        printf("destroy\n");
        if (Ptr) {
            Release();
        }
    }

    CSmartPtr &operator=(const CSmartPtr &Other) {
        printf("=\n");
        if (Ptr) {
            Release();
        }
        if (this != &Other) {
            Attach(Other);
        }
        return *this;
    }
};

int main() {
    auto ptr1 = CSmartPtr<int>();
    auto ptr2 = CSmartPtr<int>();
    ptr1.Set(new int(1));
    ptr2.Set(new int(2));
    cout << "ptrs: " << ptr1.Get() << ' ' << ptr2.Get() << endl;
    ptr1 = ptr2;
    cout << "ptrs: " << ptr1.Get() << ' ' << ptr2.Get() << endl;

}

