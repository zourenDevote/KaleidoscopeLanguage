extern GetInt() : int;
extern PutInt(int v);
extern NewLine();

def main() : int {
    int a;
    a = GetInt();
    if (a >= 0) then
        PutInt(a);
    else 
        PutInt(-a);
    NewLine();
    return 0;
}