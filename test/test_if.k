extern getint() : int;
extern putint(int v);
extern newline();

def main() : int {
    int a;
    a = getint();
    putint(a);
    newline();
    return 0;
}