int x = 3;
int m;

int func1(int a, int b) {
	int c = a + b * 5;
	return c;
}

int func2(int a, int b) {
	int c = a + b / 3;
	return c;
}

int func3() {
	int y = func1(x, 4);
	int x = func2(m, 6);
	return x + y;
}

int main() {
	int result = func3();
	return result;
}