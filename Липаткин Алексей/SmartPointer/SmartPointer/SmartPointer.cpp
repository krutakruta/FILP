#include <iostream>

using namespace std;

template <typename T>
class SmartPointer {
private:
    T* m_element;
    unsigned* ref_count;
public:
    SmartPointer() { 
        m_element = NULL;
        ref_count = NULL;
    };

    ~SmartPointer() {
        if (m_element) {
            Release();
        }
    }

    void Set(T* element) { 
        m_element = element;
        if (!ref_count) {
            ref_count = (unsigned*)malloc(sizeof(unsigned));
            *ref_count = 0;
        }
        ++(*ref_count);

    }

    T Get() { return *m_element; }

    void Release() {
        --(*ref_count);
        if (!(*ref_count)) {
            delete m_element;
            free(ref_count);
        }
    }

    SmartPointer& operator=(SmartPointer& sptr) {
        if (ref_count && *ref_count > 0) {
            Release();
        }
        if (this != &sptr) {
            m_element = sptr.m_element;
            ref_count = sptr.ref_count;
            ++(*ref_count);
        }
        return *this;
    }

    T* operator->() { return m_element; }
    
    T& operator* () { return *m_element; }
};


int main()
{
    auto integerSmartPointer = SmartPointer<int>();
    integerSmartPointer.Set(new int(3));
    cout << integerSmartPointer.Get() << '\n';
    integerSmartPointer.Release();
    integerSmartPointer.Set(new int(15));
    cout << integerSmartPointer.Get() << '\n';
    integerSmartPointer.Release();
    integerSmartPointer.Set(new int(97));
    auto newIntegerSmartPointer = SmartPointer<int>();
    newIntegerSmartPointer = integerSmartPointer;
    integerSmartPointer.Release();
    cout << *newIntegerSmartPointer << '\n';
}