# include "stupid_buffer.h"
#include <iostream>


const size_t one_buffer_size = 1024*1024;
template <typename T>
class mmap_allocator
{


public:

    static int gid;

    int id;

    using value_type = T;
    using buffer_type = stupid_buffer<T>;

    typedef size_t size_type;
    typedef T* pointer;

    buffer_type * current;
    buffer_type *start;
    int used_bytes_count;
    int buf_count;

    mmap_allocator operator=(const mmap_allocator &c) = delete;



    pointer allocate(size_type n)
    {
        int tmp = used_bytes_count;
        used_bytes_count += n* sizeof(T);
        if (used_bytes_count > one_buffer_size) {
            buf_count++;
            buffer_type *next_next = new buffer_type(one_buffer_size);
            current->set_next(next_next);
            buffer_type *prev = current;
            current = next_next;
            current->prev = prev;
        }
        return current->alloc(n);
    }

    void deallocate(pointer p, size_type n)
    {
        return current->dealloc(p,n);
    }

    mmap_allocator()
    {
        id = gid++;
        current = new buffer_type(one_buffer_size);
        start = current;
        used_bytes_count = 0;
        buf_count=1;

    }
    ~mmap_allocator()
    {
        stupid_buffer<T> *iter = start;
        for (int i = 0; i<buf_count;i++)
        {
            stupid_buffer<T> *to_del = iter;
            iter = to_del->get_next();
            delete to_del;
        }
    }

};

template <typename T>
int mmap_allocator<T>::gid = 0;