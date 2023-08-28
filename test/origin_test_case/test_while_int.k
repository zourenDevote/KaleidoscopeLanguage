
extern GetInt() : int;
extern PutInt(int d);
extern Space();
extern NewLine();

int globalArray[10];

def main() : int {
    int i;
    i = 0;
    while (i < 10) {
        globalArray[i] = GetInt();
        i = i + 1;
    }

    i = 9;
    while (i >= 0) {
        PutInt(globalArray[i]);
        Space();
        i = i - 1;
    }

    NewLine();

    return 0;
}
