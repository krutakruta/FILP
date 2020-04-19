#include <iostream>
#include "smartPtr.h"

using namespace std;

class A
{
public:
	int a = 0;

	A(int kek)
	{
		a = kek;
		cout << "A is created" << a << endl;
	}

	~A()
	{
		cout << "A is deleted" << a << endl;
	}
};

int main()
{
	smartPtr<A> a = smartPtr<A>(new A(4));
	smartPtr<A> b = smartPtr<A>(new A(3));
	smartPtr<A> c = smartPtr<A>(b);
	c.Release();

	cout << a->a << endl;
	cout << b->a << endl;
	b.Release();
	a.Release();
	system("pause");
	return 0;
}