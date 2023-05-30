
# print array use for

extern getdouble() : double;
extern putdouble(double v);
extern space();
extern newline();

double globalArray[10];

def main() : int {
    int i;
    for i = 0 , i < 10 , 1 in {
        globalArray[i] = getdouble();
    }

    for i = 0 , i < 10 , 1 in {
        putdouble(globalArray[i]);
        space();
    }

    newline();

    return 0;
}

