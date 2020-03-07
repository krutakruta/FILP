#pragma once
template<class T>
struct LinkedList {
	T* a;
	LinkedList<T>* prev;

	LinkedList() {
		prev = nullptr;
	}

	~LinkedList() {
		delete a;
		if (prev != nullptr)
			delete prev;
	}

	LinkedList(const LinkedList& x) = delete;
	LinkedList& operator=(const LinkedList& x) {
		this->prev = x->prev;
		this->a = x->a;
		return *this;
	}

	LinkedList(T* value) {
		a = value;
		prev = nullptr;
	}

	LinkedList<T>* push_back(T* value) {
		LinkedList<T>* nv = new LinkedList<T>(value);
		nv->prev = this;
		return nv;
	}
};