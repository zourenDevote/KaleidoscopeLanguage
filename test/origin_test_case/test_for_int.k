
extern GetInt() : int;
extern PutInt(int d);
extern Space();
extern NewLine();

int globalArray[10];

def main() : int {
    int i;
    for (i = 0 ; i < 10 ; i = i+1) in {
        globalArray[i] = GetInt();
    }

    for (i = 0 ; i < 10 ; i = i+1) in {
        PutInt(globalArray[i]);
        Space();
    }

    NewLine();

    return 0;
}

