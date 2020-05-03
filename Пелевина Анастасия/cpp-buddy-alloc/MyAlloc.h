#include <cmath>
#include <iostream>
#include <vector>

#include "LinkedList.h"


#define MEMORY_SIZE 1024 * 1024 * 1024
#define MIN_BLOCK 64

using namespace std;


template<class T>
class MyAlloc {
public:
  typedef size_t size_type;
  typedef T* pointer;
  typedef T value_type;

private:
  vector <LinkedList*> buddyRows;

public:

  MyAlloc() {
    for (int i = 0; i < log2(MEMORY_SIZE / MIN_BLOCK); i++)
    {
      buddyRows.push_back(new LinkedList());
    }
    
    buddyRows[buddyRows.size() - 1]->add_node(NULL);
  }

  pointer allocate(size_type n)
  {
    auto row = ceil(log2(n * sizeof(value_type)) - log2(MIN_BLOCK));
    row = row <= 0 ? 0 : row;

    while (true)
    {
      if (buddyRows[row]->get_length() > 0)
      { 
        auto tmp = buddyRows[row]->get_node(0);
        auto ptr = (pointer)tmp->ptr;
        buddyRows[row]->remove_node(0);
        return ptr;
      }
      else if (row < log2(MEMORY_SIZE / MIN_BLOCK))
      {
        row++;

        if (buddyRows[row]->get_length() > 0)
        {
          auto tmp = buddyRows[row]->get_node(0);
          free(tmp->ptr);
          buddyRows[row]->remove_node(0);
          buddyRows[row - 1]->add_node(malloc(get_block_size(row - 1) + sizeof(Node)));
          buddyRows[row - 1]->add_node(malloc(get_block_size(row - 1) + sizeof(Node)));
          row = ceil(log2(n) - log2(MIN_BLOCK));
          row = row <= 0 ? 0 : row;
        }
      }
      else
      {
        return NULL;
      }
    }
  };

  void deallocate(pointer p, size_type n)
  {
    free(p);
  };

private:
  int get_block_size(int row)
  {
    return pow(2, row) * MIN_BLOCK;
  }
};