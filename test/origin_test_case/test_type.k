
def main() : int {
    char c;
    uchar uc;
    short s;
    ushort us;
    int i;
    uint ui;
    long l;
    ulong ul;
    float f;
    double d;

    c = 'h';
    uc = -1;
    s = 1;
    us = -1;
    i = 1;
    ui = -1;
    l = 1;
    ul = -1;
    f = 2.0;
    d = 100.0;

    PrintLn("%c %u", c, uc);
    PrintLn("%d %u", s, us);
    PrintLn("%d %u", i, ui);
    PrintLn("%ld %lu", l, ul);
    PrintLn("%f", f);
    PrintLn("%lf", d);

    return 0;
}


