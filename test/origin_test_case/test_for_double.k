
extern GetDouble() : double;
extern PutDouble(double v);
extern Space();
extern NewLine();

double globalArray[10];

def main() : int {
    int i;
    for (i = 0 ; i < 10 ; i = i+1) in {
        globalArray[i] = GetDouble();
    }

    for (i = 0 ; i < 10 ; i = i+1) in {
        PutDouble(globalArray[i]);
        Space();
    }

    NewLine();

    return 0;
}

