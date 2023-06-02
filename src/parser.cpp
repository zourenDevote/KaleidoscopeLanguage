
#include "parser.h"
#include <string>
#include <stdio.h>
#include <algorithm>
#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <cassert>
#include <vector>
#include <map>
#include <set>


/// @brief 当前的ID
// static std::string IdStr;
/// @brief 当前的行列号
// static unsigned Row = 1, Col = 0;
/// @brief 存储的浮点字面常量
// static double DoubleNumVal;
/// @brief 存储的整数字面常量
// static int IntNumVal;
/// @brief 文件句柄
// static FILE *fp;
/// @brief 当前的文件
// static std::string FileName; 

// static void openFile(const char* filepath) {
//     FileName = filepath;
//     fp = fopen(filepath, "rt+");
//     if(!fp) {
//         fprintf(stderr, "Failed to open file %s\n", filepath);
//         exit(-1);
//     }
//     return;
// }

// #define LOG_ERROR(X) fprintf(stderr, "%s:%d:%d error: %s\n", FileName.c_str(), Row, Col, X); assert(false);

// /////////////////////////////////////////////////////////
// /// Lexer
// /////////////////////////////////////////////////////////

// /// 获取token
// static int LastChar = ' ';

// static int getToken() {

//     while(isspace(LastChar)) {
//         if(LastChar == '\n') {
//             Row++; Col = 0;
//         }
//         LastChar = fgetc(fp);
//         Col++;
//     }
        

//     if(isalpha(LastChar)) { // identifier: [a-zA-Z][a-zA-Z0-9]*
//         IdStr = LastChar;
//         while(isalnum((LastChar = fgetc(fp)))) {
//             IdStr += LastChar;
//             Col++;
//         }
            

//         if (IdStr == "def")
//             return tok_def;
//         if (IdStr == "extern")
//             return tok_extern;
//         if (IdStr == "if")
//             return tok_if;
//         if (IdStr == "then")
//             return tok_then;
//         if (IdStr == "else")
//             return tok_else; 
//         if (IdStr == "for")
//             return tok_for;
//         if (IdStr == "in")
//             return tok_in;
//         if (IdStr == "while")
//             return tok_while;
//         if (IdStr == "return")    
//             return tok_return;
//         if (IdStr == "double")
//             return tok_double;
//         if (IdStr == "int")
//             return tok_int;
//         if (IdStr == "true")
//             return tok_true;
//         if (IdStr == "bool")
//             return tok_bool;
//         if (IdStr == "false")
//             return tok_false;
//         if (IdStr == "void")
//             return tok_void;
//         return tok_id;
//     }

//     if (isdigit(LastChar)) {
//         std::string NumStr;
//         bool isFloat = false;
//         do {
//             NumStr += LastChar;
//             LastChar = fgetc(fp);
//             Col++;
//             if(LastChar == '.')
//                 isFloat = true;
//         } while(isdigit(LastChar) || LastChar == '.');

//         if(isFloat) {
//             DoubleNumVal = strtod(NumStr.c_str(), nullptr);
//             return tok_double_number;
//         }

//         IntNumVal = atoi(NumStr.c_str());
//         return tok_int_number;
//     }

//     switch (LastChar)
//     {
//         case '+' : LastChar = fgetc(fp); Col++; return tok_plus;
//         case '-' : LastChar = fgetc(fp); Col++; return tok_sub;
//         case '*' : LastChar = fgetc(fp); Col++; return tok_mul;
//         case '/' : LastChar = fgetc(fp); Col++; return tok_div;
//         case '>' : {
//             LastChar = fgetc(fp);
//             Col++;
//             if(LastChar == '=') {
//                 LastChar = fgetc(fp);
//                 Col++;
//                 return tok_gteq;
//             }
//             return tok_gt;
//         }
//         case '<' : {
//             LastChar = fgetc(fp);
//             Col++;
//             if(LastChar == '=') {
//                 LastChar = fgetc(fp);
//                 Col++;
//                 return tok_lteq;
//             }             
//             return tok_lt;
//         }
//         case '=' : {
//             LastChar = fgetc(fp);
//             Col++;
//             if(LastChar == '=') {
//                 LastChar = fgetc(fp);
//                 Col++;
//                 return tok_eq;
//             }
//             return tok_assign;
//         }
//         case '!' : {
//             LastChar = fgetc(fp);
//             Col++;
//             if(LastChar == '=') {
//                 LastChar = fgetc(fp);
//                 Col++;
//                 return tok_neq;
//             }
//             return tok_not;
//         }
//     }

//     if(LastChar == '#') {
//         do {
//             LastChar = fgetc(fp);
//             Col++;
//         }
//         while(LastChar != EOF && LastChar != '\n');

//         if(LastChar != EOF)
//             return getToken();
//     }

//     if(LastChar == EOF)
//         return tok_eof; 

//     int ThisChar = LastChar;
//     LastChar = fgetc(fp);
//     Col++;
//     return ThisChar;
// }

// void unitTest(const char* filepath) {
//     openFile(filepath);
//     int tok;
//     tok = getToken();
//     while(tok != tok_eof) {
//         printf("Token value = %d, Row %d, Col %d\n", tok, Row, Col);
//         tok = getToken();
//     }
// }

// static int CurTok;
// static int CurrSeek;
// static int getNextToken() { return CurTok = getToken(); }

/// 前看N个token
// std::vector<int> lookUp(int n) {
//     std::vector<int> LookUpToks;
//     int CurrentLoc = ftell(fp);
//     int SavedChar = LastChar;
//     unsigned SavedRow = Row, SavedCol = Col;
//     int i = 0, tok;
//     while (i < n && CurTok != tok_eof) {
//         tok = getToken();
//         LookUpToks.push_back(tok);
//         i++;
//     }
//     fseek(fp, CurrentLoc, SEEK_SET);  
//     LastChar = SavedChar;
//     Row = SavedRow; Col = SavedCol;

//     return LookUpToks;
// }



