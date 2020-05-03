// BuddyAllocator.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#include <iostream>
#include <windows.h>
#include <sstream>
#include <map>
#include <chrono>
#include "buddyAlloc.h"
using namespace std;


#define DELIMITERS " .\n\r, ? !'\":;//()-_"
#define BOOK_NAME L"book.txt"


char* ReadFile() {
	HANDLE file = CreateFile(BOOK_NAME, GENERIC_READ, 0, NULL,
		OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (file == INVALID_HANDLE_VALUE)
		std::cout << "error";
	LARGE_INTEGER fileSize;
	GetFileSizeEx(file, &fileSize);
	char* text = new char[fileSize.QuadPart + 1];
	DWORD dwTemp;

	ReadFile(file, text, fileSize.QuadPart, &dwTemp, NULL);
	CloseHandle(file);
	return text;
}

map<string, int, less<string>, BuddyAlloc<pair<string, int>>> split_text_for_custom_allocator(char* text) {
	int a = 10;
	char* next_toc = NULL;
	map<string, int, less<string>, BuddyAlloc<pair<string, int>>> words;
	char* chars_array = strtok_s(text, DELIMITERS, &next_toc);
	while (chars_array)
	{
		map <string, int> ::iterator pos;
		for (int i = 0; i <= strlen(chars_array); i++) {
			chars_array[i] = tolower(chars_array[i]);
		}
		pos = words.find(chars_array);
		if (pos == words.end()) {
			words[chars_array] = 1;
		}
		else {
			pos->second++;
		}
		chars_array = strtok_s(NULL, DELIMITERS, &next_toc);
	}
	return words;
}

map<string, int> split_text_for_standart_allocator(char* text) {
	int a = 10;
	char* next_toc = NULL;
	map<string, int> words;
	char* chars_array = strtok_s(text, DELIMITERS, &next_toc);
	while (chars_array)
	{
		map <string, int> ::iterator pos;
		for (int i = 0; i <= strlen(chars_array); i++) {
			chars_array[i] = tolower(chars_array[i]);
		}
		pos = words.find(chars_array);
		if (pos == words.end()) {
			words[chars_array] = 1;
		}
		else {
			pos->second++;
		}
		chars_array = strtok_s(NULL, DELIMITERS, &next_toc);
	}
	return words;
}

int main()
{
	char* text = ReadFile();
	auto start_c = std::chrono::high_resolution_clock::now();
	map<string, int, less<string>, BuddyAlloc<pair<string, int>>> words_custom = split_text_for_custom_allocator(text);
	auto stop_c = std::chrono::high_resolution_clock::now();
	delete[] text;
	char* text3 = ReadFile();
	auto start_s = std::chrono::high_resolution_clock::now();
	map<string, int> words_standart = split_text_for_standart_allocator(text3);
	auto stop_s = std::chrono::high_resolution_clock::now();
	delete[] text3;
	auto c_time = chrono::duration_cast<chrono::microseconds>(stop_c - start_c);
	auto s_time = chrono::duration_cast<chrono::microseconds>(stop_s - start_s);


	map<string, int> ::iterator it = words_custom.begin();
	for (; it != words_custom.end(); it++) {
		cout << it->first << ' ' << it->second << '\n';
	}
	cout << "custom allocator time: " << c_time.count() << "ms" << endl;
	cout << "standart allocator time: " << s_time.count() << "ms" << endl;
}

// Запуск программы: CTRL+F5 или меню "Отладка" > "Запуск без отладки"
// Отладка программы: F5 или меню "Отладка" > "Запустить отладку"

// Советы по началу работы 
//   1. В окне обозревателя решений можно добавлять файлы и управлять ими.
//   2. В окне Team Explorer можно подключиться к системе управления версиями.
//   3. В окне "Выходные данные" можно просматривать выходные данные сборки и другие сообщения.
//   4. В окне "Список ошибок" можно просматривать ошибки.
//   5. Последовательно выберите пункты меню "Проект" > "Добавить новый элемент", чтобы создать файлы кода, или "Проект" > "Добавить существующий элемент", чтобы добавить в проект существующие файлы кода.
//   6. Чтобы снова открыть этот проект позже, выберите пункты меню "Файл" > "Открыть" > "Проект" и выберите SLN-файл.
