#include<iostream>
#include<bitset>

using namespace std;

const size_t MIN_ORD = 4;
const size_t MAX_ORD = 6;
const size_t MAX_SIZE = 1 << 3;

struct block
{
	void* addr;
	block* next;
	size_t size;

	block(size_t s)
	{
		next = nullptr;
		size = s;
	}
};

struct linked_list
{
	block* head;
	block* tail;
	size_t count;
	size_t block_size;

	linked_list()
	{
		head = nullptr;
		tail = nullptr;
		count = 0;
		block_size = 0;
	}

	void push_back_addressed(void* memory)
	{
		block* new_element = new block(block_size);
		if (count == 0)
		{
			head = new_element;
			tail = new_element;
			head->addr = memory;
		}
		else
		{
			void* addr = tail->addr;
			tail->next = new_element; 
			tail = new_element;
			tail->addr = memory;
		}
		count++;
	}

	void push_back(block* buddy)
	{
		if (count == 0)
		{
			head = buddy;
			tail = buddy;
		}
		else
		{
			tail->next = buddy;
			tail = buddy;
		}
		count++;
	}

	void remove(block* elem)
	{
		if (count == 0)
			return;

		if (elem->addr == head->addr)
		{
			head = head->next;
			if (count == 1)
				tail = nullptr;
			count--;
			return;
		}

		block* current = head;
		while (current->next->addr != elem->addr)
		{
			current = head->next;
			if (current->next == nullptr)
				return;
		}

		block* found = current->next;
		current->next = found->next;
		if (found == tail)
			tail = current;
		count--;
	}

	block* pop_front()
	{
		block* return_value = head;
		if (count > 0)
		{
			head = head->next;
			count--;
		}
		return return_value;
	}

	block* get_block_at_position(size_t position)
	{
		block* current = head;
		for (int i = 1; i < position; ++i)
			current = head->next;
		return current;
	}
};

template<size_t N>
struct buddy_system
{
private:
	size_t memory_size;
	size_t min_ord;
	size_t max_ord;
	
public:
	void* memory;
	bitset<N> flags; //true - зан€т, false - свободен
	linked_list** free_spaces;

	buddy_system(size_t min_order, size_t max_order)
	{
		memory_size = 1 << max_order;
		memory = malloc(memory_size);
		min_ord = min_order;
		max_ord = max_order;
		free_spaces = new linked_list*[max_order - min_order + 1];
		size_t current_size = 1 << min_order;

		//¬ начале имеем только один блок(вс€ выделенна€ пам€ть)
		for (int i = min_order; i <= max_order; ++i)
		{
			free_spaces[i - min_order] = new linked_list();
			free_spaces[i - min_order]->block_size = current_size;
			current_size = current_size << 1;
		}
		free_spaces[max_ord - min_ord]->push_back_addressed(memory);
		flags.reset();
	}

	~buddy_system()
	{
		delete memory;
		delete[] free_spaces;
	}

	void* allocate(size_t size)
	{
		for (int i = min_ord; i <= max_ord; ++i)
		{
			if (free_spaces[i - min_ord]->block_size >= size)
			{
				//≈сли уже есть свободный блок нужного размера
				if (free_spaces[i - min_ord]->count > 0)
				{
					block* elem = free_spaces[i - min_ord]->pop_front();
					printf("Allocated memory at %d level, %d bytes, %p address\n", i - min_ord, free_spaces[i - min_ord]->block_size, elem->addr);
					int index = (memory_size / free_spaces[i - min_ord]->block_size - 1) + ((static_cast<char*>(elem->addr) - static_cast<char*>(memory)) / free_spaces[i - min_ord]->block_size);
					flags[index] = true;
					return elem->addr;
				}
				//≈сли его нет, то находим больший свободный блок и делим его до нужного размера
				else
				{
					for (int index = i - min_ord + 1; index <= max_ord - i; ++index)
					{
						if (free_spaces[index]->count > 0)
						{
							void* addr = free_spaces[index]->pop_front()->addr;

							int ind = (memory_size / free_spaces[index]->block_size - 1) + ((static_cast<char*>(addr) - static_cast<char*>(memory)) / free_spaces[index]->block_size);
							flags[ind] = true;

							int deep_index = index - 1;
							while (deep_index >= i - min_ord && deep_index >= 0)
							{
								free_spaces[deep_index]->push_back_addressed(addr);
								addr = static_cast<char*>(addr) + free_spaces[deep_index]->block_size;
								int ind = (memory_size / free_spaces[deep_index]->block_size - 1) + ((static_cast<char*>(addr) - static_cast<char*>(memory)) / free_spaces[deep_index]->block_size);
								flags[ind] = true;
								deep_index--;
							}
							printf("Allocated memory at %d level, %d bytes, %p address\n", deep_index + 1, free_spaces[deep_index + 1]->block_size, addr);
							return addr;
						}
					}
				}
			}
		}
		printf("Memory is full\n");
		return nullptr;
	}

	void deallocate(block* buddy)
	{
		block* current = buddy;
		int depth = memory_size / (1 << min_ord) - memory_size / current->size;
		int index = (memory_size / free_spaces[depth]->block_size - 1) + ((static_cast<char*>(current->addr) - static_cast<char*>(memory)) / free_spaces[depth]->block_size);

		while (true)
		{
			free_spaces[depth]->push_back(current);
			flags[index] = false;
			printf("Block at %d level with %p address was cleared\n", depth, current->addr);
			if (index > 0)
			{
				if (index % 2 == 0)
				{
					if (!flags[index - 1])
					{
						block* neighbour = new block(current->size);
						neighbour->addr = static_cast<char*>(current->addr) - current->size;

						block* new_block = new block(current->size * 2);
						new_block->addr = static_cast<char*>(current->addr) - current->size;
					
						printf("Blocks at %d level with %p and %p addresses was merged\n", depth, current->addr, neighbour->addr);

						current = new_block;
					}
					else
						break;
				}
				else
				{
					if (!flags[index + 1])
					{
						block* neighbour = new block(current->size);
						neighbour->addr = static_cast<char*>(current->addr) + current->size;

						block* new_block = new block(current->size * 2);
						new_block->addr = static_cast<char*>(current->addr);

						printf("Blocks at %d level with %p and %p addresses was merged\n", depth, current->addr, neighbour->addr);

						current = new_block;
					}
					else
						break;
				}
				index = (index - 1) / 2;
				depth++;
			}
			else
				break;
		}
	}
};

void test()
{
	buddy_system<MAX_SIZE>* buddies = new buddy_system<MAX_SIZE>(MIN_ORD, MAX_ORD);
	buddies->allocate(1);
	buddies->allocate(1);
	buddies->allocate(32);
	buddies->allocate(1);

	block* buddy = new block(16);
	buddy->addr = static_cast<char*>(buddies->memory) + 32;
	buddies->deallocate(buddy);

	buddy->addr = static_cast<char*>(buddies->memory) + 48;

	buddies->deallocate(buddy);

	buddies->allocate(32);
}

int main()
{
	test();
	return 0;
}