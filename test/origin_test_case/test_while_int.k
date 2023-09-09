
int globalArray[10];

def main() : int {
    int i;
    int sum;
    i = 0;
    sum = 0;
    while (i < 10) {
        globalArray[i] = sum + i;
        i = i + 1;
    }

    i = 9;
    while (i >= 0) {
        Print("%d ", globalArray[i]);
        i = i - 1;
    }
    PrintLn("");

    return 0;
}
