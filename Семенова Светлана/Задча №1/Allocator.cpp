#include <iostream>
#include <Windows.h>
#include <map>
#include <vector>
#include <algorithm>
#include <ctime>

using namespace std;

template <class T>
struct Block
{
	Block<T>* next;
	char* data;
	size_t size;
	size_t offset;
};


template<class T>
struct MyAllocator 
{
	using value_type = T;
	Block<T>* startBlock;
	unsigned int sizeBlock = 1024 * 1024;

	template< class U >
	struct rebind 
	{
		typedef MyAllocator<U> other;
	};

	Block<T>* createBlock()
	{
		auto ptr = malloc(sizeof(Block<T>) + sizeBlock);
		if (ptr)
		{
			Block<T>* block = reinterpret_cast<Block<T>*>(ptr);
			block->data = reinterpret_cast<char*>(ptr) + sizeof(Block<T>);
			block->next = nullptr;
			block->offset = 0;
			block->size = sizeBlock;
			return block;
		}
		cout << "Can't create block";
	}

	MyAllocator()
	{
		startBlock = nullptr;
	};

	~MyAllocator()
	{
		Block<T>* currentBlock = startBlock;
		Block<T>* oldBlock;
		while (currentBlock->next != nullptr)
		{
			oldBlock = currentBlock;
			currentBlock = currentBlock->next;
			free(oldBlock);
		}
	};

	template<class U>
	MyAllocator(const MyAllocator<U>& other) 
	{
		startBlock = (Block<T>*)other.startBlock;
	}

	T* allocate(size_t n)
	{
		if (startBlock == nullptr)
			startBlock = createBlock();
		auto currentBlock = startBlock;
		auto sizeForAlloc = n * sizeof(T);
		while (sizeForAlloc + currentBlock->offset > currentBlock->size)
		{
			if (currentBlock->next == nullptr)
				currentBlock->next = createBlock();
			currentBlock = currentBlock->next;
		}
		currentBlock->offset += n * sizeof(T);
		return reinterpret_cast<T*> (currentBlock->offset + currentBlock->data);
	}

	void deallocate(T* p, size_t n)
	{ }
};


size_t getLength(char* word) 
{
	size_t length = 0;
	while (true) 
	{
		if (!(word[length] >= 0 && isalpha(word[length])))
			return length;
		length++;
	}
}

struct MyComparator
{
	bool operator() (char* a, char* b) const
	{
		while (*a >= 0 && isalpha(*a) || *b >= 0 && isalpha(*b)) 
		{
			auto la = tolower(*a);
			auto lb = tolower(*b);
			if (la != lb)
				return la - lb < 0;
			a++;
			b++;
		}
		return false;
	}
};

bool cmp(const pair<string, size_t> a, const pair<string, size_t> b) {
	return a.second > b.second;
}


struct File
{
	HANDLE hFile;
	size_t size;
	char* Text;
	File()
	{
		hFile = CreateFile("F:\\C++\\text.txt", GENERIC_READ,
			FILE_SHARE_READ, nullptr, OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL, nullptr);
		if (hFile == INVALID_HANDLE_VALUE)
			cout << "INVALID_HANDLE_VALUE\n";
	}

	void getSize()
	{
		size = GetFileSize(hFile, nullptr);
		cout << size << std::endl;
	}

	void readFile()
	{
		Text = new char[size + 1];
		DWORD r = 0;
		bool bResult = ReadFile(hFile, Text, size, &r, nullptr);
		if (!bResult)
			cout << "Can't read file\n";
		Text[size] = 0;
	}

	void close()
	{
		CloseHandle(hFile);
	}
};

void printResultsMyAllocator(char* text, int size)
{
	unsigned int startTime = clock();
	auto wordCounter = map<char*, size_t, MyComparator, MyAllocator<char*>>();
	auto meetSpace = true;
	for (auto i = 0; i <= size; i++)
	{
		if (!(text[i] >= 0 && isalpha(text[i])))
			meetSpace = true;
		else
		{
			if (meetSpace)
				wordCounter[text + i]++;
			meetSpace = false;
		}
	}
	vector<pair<char*, size_t>> pairs(wordCounter.begin(), wordCounter.end());
	sort(pairs.begin(), pairs.end(), cmp);
	unsigned int endTime = clock();
	for (auto pair : pairs)
	{
		auto wordLength = getLength(pair.first);
		for (int i = 0; i < wordLength; i++)
			cout << pair.first[i];
		cout << ' ' << pair.second << "\n";
	}
	printf("Time StandartAllocator: %d seconds", endTime - startTime);
}

void printResultsStandartAllocator(char* text, int size)
{
	unsigned int startTime = clock();
	auto wordCounter = map<string, size_t>();
	string lastWord = "";
	for (auto i = 0; i <= size; i++)
	{
		if (text[i] >= 0 && isalpha(text[i]))
			lastWord += tolower(text[i]);
		else if (lastWord != "")
		{
			wordCounter[lastWord]++;
			lastWord = "";
		}
	}	
	vector<pair<string, size_t>> pairs(wordCounter.begin(), wordCounter.end());
	sort(pairs.begin(), pairs.end(), cmp);
	unsigned int endTime = clock();
	for (auto pair : pairs)
		cout << pair.first << ' ' << pair.second << "\n";
	printf("Time StandartAllocator: %d seconds", endTime - startTime);
}

int main() 
{
	auto file = new File();
	file->getSize();
	file->readFile();
	printResultsMyAllocator(file->Text, file->size);
	cout << "\n";
	printResultsStandartAllocator(file->Text, file->size);
	file->close();
	delete[] file->Text;
	return 0;
}
