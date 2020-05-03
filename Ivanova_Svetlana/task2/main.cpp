#include <cstring>
#include <iostream>
# include "buddy_alloc.h"


int main(int argc, char *argv[]) {
    auto buf = new buddy_allocator<char>(32);
    char* s = buf->allocate(12);
    memcpy(s, "aloha\n", 6);
    cout << s;
    buf -> deallocate(s,6);
    cout << s;
    return 0;
}