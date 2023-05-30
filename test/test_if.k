extern getint() : int;
extern putint(int v);
extern newline();

def main() : int {
    int a;
    a = getint();
    if a >= 0 then
        putint(a);
    else 
        putint(-a);
    newline();
    return 0;
}