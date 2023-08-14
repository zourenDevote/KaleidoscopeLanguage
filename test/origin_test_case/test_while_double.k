
extern getdouble() : double;
extern putdouble(double d);
extern space();
extern newline();

double globalArray[10];

def main() : int {
    int i;
    i = 0;
    while (i < 10) {
        globalArray[i] = getdouble();
        i = i + 1;
    }

    i = 9;
    while (i >= 0) {
        putdouble(globalArray[i]);
        space();
        i = i - 1;
    }

    newline();

    return 0;
}
