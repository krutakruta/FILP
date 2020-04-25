#ifndef CUSTOMALLOCATOR_H
#define CUSTOMALLOCATOR_H

#include <iostream>
#include "LinkedList.h"

const int defaultSize = 1024 * 1024;

template<typename T>
struct CustomAllocator {
    using value_type = T;
    using pointer = T *;
    using const_pointer = const T *;
    using reference = T &;
    using const_reference = const T &;
    using size_type = size_t;

    LinkedList<T> *linkedList;

    CustomAllocator() noexcept {
        linkedList = new LinkedList<T>();
        linkedList->shift(new T[defaultSize]);
    }

    CustomAllocator(const CustomAllocator &other) noexcept {
        linkedList = new LinkedList<T>();
        linkedList->shift(new T[defaultSize]);
    }

    template<typename U>
    explicit CustomAllocator(const CustomAllocator<U> &other) noexcept {
        linkedList = new LinkedList<T>();
        linkedList->shift(new T[defaultSize]);
    }

    template<typename U>
    struct rebind {
        typedef CustomAllocator<U> other;
    };

    ~CustomAllocator() noexcept {
        while (linkedList->size() > 0) {
            linkedList->unshift();
        }
    };

    pointer allocate(size_type n) {
        if (linkedList->get(0) + n * sizeof(T) > linkedList->get(0) + defaultSize) {
            T *value = new T[defaultSize];
            linkedList->shift(value);
        }
        T *result = linkedList->unshift();
        linkedList->shift(result + n * sizeof(T));
        return reinterpret_cast<pointer>(result);
    }

    void deallocate(pointer p, size_type n) {}

    size_t max_size() const {
        return defaultSize / sizeof(T);
    }

    void construct(pointer ptr, const_reference val) {
        new(ptr) T(val);
    }

    void destroy(pointer ptr) {
        ptr->~T();
    }
};

#endif
