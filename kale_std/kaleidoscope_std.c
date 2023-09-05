
#include "kaleidoscope_std.h"
#include <stdio.h>
#include <stdarg.h>

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
void Print(const char *format, ...) {
    va_list args;
    va_start(args, format);
    vprintf(format, args);
    va_end(args);
}

void PrintLn(const char *format, ...) {
    va_list args;
    va_start(args, format);
    vprintf(format, args);
    va_end(args);
    printf("\n");
}

