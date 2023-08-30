
extern GetDouble() : double;
extern PutDouble(double d);
extern Space();
extern NewLine();

double globalArray[10];

def main() : int {
    int i;
    i = 0;
    while (i < 10) {
        globalArray[i] = GetDouble();
        i = i + 1;
    }

    i = 9;
    while (i >= 0) {
        PutDouble(globalArray[i]);
        Space();
        i = i - 1;
    }

    NewLine();

    return 0;
}
