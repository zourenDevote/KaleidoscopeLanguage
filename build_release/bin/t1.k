


int globalArray[10];

def main() : int {
    int i;
    int sum;
    sum = 0;
    for (i = 0 ; i < 10 ; i = i+1) in {
        globalArray[i] = sum + i;
    }

    for (i = 0 ; i < 10 ; i = i+1) in {
        Print("%d ", globalArray[i]);
    }
    PrintLn("");

    return 0;
}

