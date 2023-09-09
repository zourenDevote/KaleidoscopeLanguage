
double globalArray[10];

def main() : int {
    int i;
    double sum;
    sum = 0.0;
    i = 0;
    while (i < 10) {
        sum = sum + i;
        globalArray[i] = sum;
        i = i + 1;
    }

    i = 9;
    while (i >= 0) {
        Print("%f ", globalArray[i]);
        i = i - 1;
    }
    PrintLn("");

    return 0;
}
