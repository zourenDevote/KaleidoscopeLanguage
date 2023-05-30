extern getint() : int;
extern putint(int n);
extern newline();

def fibonacci(int n) : int {
    if n <= 1 then
        return 1;
    return fibonacci(n-1) + fibonacci(n-2);
}

def main() : int {
    int a;
    a = getint();
    putint(fibonacci(a));
    newline();
}