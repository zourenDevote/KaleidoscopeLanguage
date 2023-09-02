
extern PutInt(int t);
extern Space();
extern NewLine();

def main() : int {
    int i, j;
    i = 10;
    for (j = 0 ; j < i ; j = j + 1) in {
        PutInt(j);
        Space();
        if (j >= i - 2) then {
           break;
        }
    }
    NewLine();
    return 0;
}

