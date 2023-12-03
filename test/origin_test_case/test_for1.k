def main() : int {
    int i, j;
    i = 10;
    for (j = 0 ; j < i ; j = j + 1) in {
        Print("%d ", j);
        if (j >= i - 2) then {
           break;
        }
    }
    PrintLn("");
    return 0;
}