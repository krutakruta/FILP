#include <windows.h>
#include <iostream>
#include <map>
#include <time.h>
#include <string>
using namespace std;

struct myComp {
	bool operator() (const char* first, const char* second) const
	{
		int t = 0;
		while (*(first + t) >= 0 && isalpha(*(first + t)) || *(second + t) >= 0 && isalpha(*(second + t))) {
			char a = tolower(*(first + t));
			char b = tolower(*(second + t));
			if (a != b)
				return a - b < 0;
			t++;
		}
		return false;
	}
};

struct CItem {
	CItem* Next;
	CItem* Prev;
	char* cTop;
	char* Data;
	CItem() {
		Data = new char[1024*1024];
		cTop = Data;
	}
	~CItem() {
		if (Data != 0) {
			delete Data;
			Data = 0;
			Next->~CItem();
			Next = 0;
			Prev = 0;
		}
	}
};


struct DoubleLinkedList {
	CItem *Free;
	DoubleLinkedList() {
		Free = new CItem();
		Free->Next = Free;
		Free->Prev = Free;
	}
	~DoubleLinkedList() { }
	void Insert(CItem* New) {
		CItem* Left = Free->Prev;
		Left->Next = New;
		New->Prev = Left;
		New->Next = Free;
		Free->Prev = New;
		Free = New;
	}
};

template<class T>
struct myAlloc{
	typedef typename T value_type;
	typedef size_t size_type;
	typedef ptrdiff_t difference_type;
	typedef value_type* point;

	typedef const T* const_pointer;
	typedef T* pointer;
	typedef value_type& reference;
	typedef const T& const_reference;

	myAlloc() {	}

	template< class U >
	struct rebind {
		typedef myAlloc<U> other;
	};

	myAlloc(const myAlloc& other) noexcept = default;

	template< class U >
	myAlloc(const myAlloc<U>& other) noexcept
	{}

	~myAlloc() { }

	pointer allocate(size_type n, void* hint = 0) {
		if (list.Free->cTop + n * sizeof(T) > list.Free->Data + 1024*1024) {
			CItem * New = new CItem();
			list.Insert(New);
		}
		char *result = list.Free->cTop;
		list.Free->cTop += n*sizeof(T);
		return reinterpret_cast<pointer>(result);
	}
	void deallocate(pointer p, size_type n) { }
	size_t max_size() const
	{
		return 1024*1024  / sizeof(T);
	}

	void construct(pointer p, const_reference val)
	{
		new(p) T(val);
	}

	void destroy(pointer p) {
		p->~T(); 
	}
	DoubleLinkedList list;
};


int main(int argc, char *argv[]) {
	const char *filename;
	if (argc <= 2)
		return 1;
	filename = argv[2];
	unsigned long Size = 0;
	HANDLE hFile = CreateFile(filename, GENERIC_READ, FILE_SHARE_READ, NULL,
		OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	Size = GetFileSize(hFile, NULL);
	std::cout << Size << std::endl;

	char *Text;
	Text = new char[Size + 1];
	if(!ReadFile(hFile, Text, Size, NULL, NULL))
		return 1;
	CloseHandle(hFile);

	float customMapTime1 = clock() / (float)CLOCKS_PER_SEC;
	map <char*, size_t, myComp, myAlloc<char*>> Map;
	int t = 0;
	for (char *i = Text; i < &Text[Size]; i++) {
		int a = int(*i);
		if (a >= 0 && isalpha(*i)) {
			t++;
		}
		else if (t > 0) {
			Map[(i - t)]++;
			t = 0;
		}
	}
	float customMapTime2 = clock() / (float)CLOCKS_PER_SEC;

	float regMapTime1 = clock() / (float)CLOCKS_PER_SEC;
	map <string, size_t> regularMap;
	t = 0;
	for (int i = 0; i < Size; i++) {
		int a = int(Text[i]);
		if (a >= 0 && isalpha(int(Text[i]))) {
			t++;
		}
		else if (t > 0) {
			string temp = "";
			for (int j = i - t; j < i; j++)
				temp += tolower(Text[j]);
			regularMap[temp]++;
			t = 0;
		}
	}
	float regMapTime2 = clock() / (float)CLOCKS_PER_SEC;
	std::cout << "Regular map time: " << regMapTime2 - regMapTime1 << endl;
	std::cout << "Custom map time: " << customMapTime2 - customMapTime1 << endl;

	if (regMapTime2 - regMapTime1 > customMapTime2 - customMapTime1)
		std::cout << "Custom map wins" << endl;
	else
		std::cout << "Regular map wins" << endl;

	multimap<size_t, string, std::greater<size_t>> reversedMap;
	for (pair<string, size_t> items : regularMap) {
		reversedMap.insert(pair<size_t, string>(items.second, items.first));
	}

	multimap<size_t, string>::iterator rit;
	for (rit = reversedMap.begin(); rit != reversedMap.end(); rit++) {
		std::cout << rit->second << ": " << rit->first << endl;
	}
	return 0;
}
