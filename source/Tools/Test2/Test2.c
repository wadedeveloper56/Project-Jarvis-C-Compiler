int x = 3;

int add(int a, int b) {
  int c = a + b * 2;
  return c;
}

int sub() {
  int c = 7;
  return c;
}

int main() {
  int y = add(x, 4);
  int z = sub(9);
  return y+z;
}

