
extern getint() : int;
extern putint(int d);
extern space();
extern newline();

int globalArray[10];

def main() : int {
    int i;
    for (i = 0 ; i < 10 ; i = i+1) in {
        globalArray[i] = getint();
    }

    for (i = 0 ; i < 10 ; i = i+1) in {
        putint(globalArray[i]);
        space();
    }

    newline();

    return 0;
}

