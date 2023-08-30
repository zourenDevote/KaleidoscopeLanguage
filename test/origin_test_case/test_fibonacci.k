extern GetInt() : int;
extern PutInt(int n);
extern NewLine();

def fibonacci(int n) : int {
    if (n <= 1) then
        return 1;
    return fibonacci(n-1) + fibonacci(n-2);
}

def main() : int {
    int a;
    a = GetInt();
    PutInt(fibonacci(a));
    NewLine();
}