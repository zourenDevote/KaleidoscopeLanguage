
double globalArray[10];

def main() : int {
    int i;
    double sum;
    sum = 0.0;
    for (i = 0 ; i < 10 ; i = i+1) in {
        globalArray[i] = sum + i;
    }

    for (i = 0 ; i < 10 ; i = i+1) in {
        Print("%f ", globalArray[i]);
    }
    PrintLn("");

    return 0;
}

