
double globalArray[10];

def main() : int {
    int i;
    double sum;
    sum = 0.0;
    i = 0;
    while (i < 10) {
        globalArray[i] = sum + i;
        i = i + 1;
    }

    i = 10;
    while (i > 0) {
        i = i - 1;
        Print("%f ", globalArray[i]);
    }
    PrintLn("");

    return 0;
}
