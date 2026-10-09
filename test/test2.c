int u;
int x = 3;
int m;

int func1(int a, int b) {
	int c;  
	c=a + b * 5;
	return c;
}

int func2(int a, int b) {
	int c;
	c = a + b / 3;
	return c;
}

int func3() {
	int a;
	int b;
	a = func1(x, 4);
	b = func2(m, 6);
	return a+b;
}
