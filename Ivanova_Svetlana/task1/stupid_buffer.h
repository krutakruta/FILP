#include <stack>
using namespace std;
template<class T>
struct stupid_buffer {

private:
    stupid_buffer<T> *next;
    char *memory_ptr;
    char *start_memory_ptr;

public:
    using value_type = T;
    stupid_buffer<T> *prev;

    void set_next(stupid_buffer<T> *value)
    {
        next = value;
    }
    stupid_buffer<T>* get_next()
    {
        return next;
    }

    stupid_buffer<T>(size_t size) {
        memory_ptr = new char[size];
        start_memory_ptr = memory_ptr;
        next = NULL;
    }

    ~stupid_buffer<T>() {
        free(start_memory_ptr);
    }

    //stupid_buffer operator=(const stupid_buffer &c) = delete;


    T* alloc(size_t n) {

        T* _Ptr = reinterpret_cast<T*>(memory_ptr);
        memory_ptr += sizeof(T) * n;
        return _Ptr;
    }

    void dealloc(T *p, size_t n) {
        /* * * */
    }

};