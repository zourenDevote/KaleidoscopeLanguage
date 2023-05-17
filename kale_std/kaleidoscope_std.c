
#include "kaleidoscope_std.h"
#include <stdio.h>

/// @brief kaleidoscope 标准库函数 获取整型变量，获取浮点变量
/// @return 
int GetInt() {
    int value;
    scanf("%d", &value);
    return value;
}

double GetDouble() {
    double value;
    scanf("%lf", &value);
    return value;
}

/// @brief kaleidoscope 标准库函数 打印整型变量，打印浮点变量，换行, 空格
void PutInt(int v) {
    printf("%d", v);
}

void PutDouble(double v) {
    printf("%f", v);
}

void NewLine() {
    printf("\n");
}

void Space() {
    printf(" ");
}

void PutChar(int v) {
    char c = (char)v;
    printf("%c", c);
}

