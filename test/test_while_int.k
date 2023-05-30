
extern getint() : int;
extern putint(int d);
extern space();
extern newline();

int globalArray[10];

def main() : int {
    int i;
    i = 0;
    while i < 10 in {
        globalArray[i] = getint();
        i = i + 1;
    }

    i = 9;
    while i >= 0 in {
        putint(globalArray[i]);
        space();
        i = i - 1;
    }

    newline();

    return 0;
}
