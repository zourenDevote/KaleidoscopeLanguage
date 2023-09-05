

def fibonacci(long n) : long {
    if (n <= 2) then
        return 1;
    return fibonacci(n-1) + fibonacci(n-2);
}

def main() : int {
    long a, i;
    a = 10;
    for (i = 1;i <= a;i=i+1) in {
       PrintLn("Fibonacci(%d) = %d", i, fibonacci(i));
    }
    return 0;
}