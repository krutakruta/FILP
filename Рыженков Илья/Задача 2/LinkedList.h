#pragma once

using namespace std;

template<class T>
class LinkedList
{
	struct ListNode
	{
		T item;
		ListNode* next;
		ListNode* prev;

		ListNode(T obj)
		{
			item = obj;
			next = nullptr;
			prev = nullptr;
		}
	};

public:
	ListNode* first = nullptr;
	ListNode* last = nullptr;
	int size = 0;

	void add(T item)
	{
		size++;
		ListNode* node = new ListNode(item);
		if (first == nullptr)
		{
			first = node;
			last = node;
		}
		else
		{
			last->next = node;
			node->prev = last;
			last = node;
		}

	}

	void pop_first()
	{
		if (size == 0)
		{
			throw "List in empty";
		}
		if (size == 1)
		{
			ListNode* to_delete = first;
			first = nullptr;
			last = nullptr;
			delete to_delete;
			size = 0;
			return;
		}
		ListNode* to_delete = first;
		first = first->next;
		first->prev = nullptr;
		size--;
		delete to_delete;
	}

	void pop(T item)
	{
		ListNode* node = find_node(item);
		if (node == nullptr)
		{
			throw "No such item in list";
		}

		size--;
		if (last == node && first == node)
		{
			first = nullptr;
			last = nullptr;
			delete node;
			return;
		}

		if (node == last)
		{
			last = node->prev;
			last->next = nullptr;
			delete node;
			return;
		}

		if (node == first)
		{
			first = node->next;
			first->prev = nullptr;
			delete node;
			return;
		}

		node->prev->next = node->next;
		node->next->prev = node->prev;
		delete node;
	}

	bool contains(T item)
	{
		ListNode* node = find_node(item);
		return node != nullptr;
	}

	~LinkedList()
	{
		ListNode* current = first;
		if (size == 0)
		{
			delete this;
			return;
		}

		if (size == 1)
		{
			delete first;
			delete this;
			return;
		}

		while (current != last)
		{
			ListNode* next = current->next;
			delete current;
			current = next;
		}
		delete this;
		delete last;
	}

private:
	ListNode* find_node(T item)
	{
		if (size == 0)
		{
			return nullptr;
		}

		ListNode* current = first;
		while (current != last)
		{
			if (current->item == item)
			{
				return current;
			}
			current = current->next;
		}
		if (last->item == item)
		{
			return last;
		}
		else
		{
			return nullptr;
		}
	}
};