// sample semantic tests for analyzer

struct Point { int x; int y; };

int add(int a, int b) { return a + b; }

int bad_return() { return 3.14; }

int* make_ptr() { int a = 1; return &a; }

void test()
{
	int i;
	int* p = &i;
	double d = 3.14;
	i = d; // type mismatch

	int arr[4];
	arr[0] = 1;

	struct Point pt;
	pt.x = 10;
	pt.y = 20;

	int r = add(1, 2);
	int s = add(1); // arg count mismatch
}
