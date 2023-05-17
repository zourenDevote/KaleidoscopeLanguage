
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
static std::string IdStr;
/// @brief 当前的行列号
static unsigned Row = 1, Col = 0;
/// @brief 存储的浮点字面常量
static double DoubleNumVal;
/// @brief 存储的整数字面常量
static int IntNumVal;
/// @brief 文件句柄
static FILE *fp;
/// @brief 当前的文件
static std::string FileName; 

static void openFile(const char* filepath) {
    FileName = filepath;
    fp = fopen(filepath, "rt+");
    if(!fp) {
        fprintf(stderr, "Failed to open file %s\n", filepath);
        exit(-1);
    }
    return;
}

#define LOG_ERROR(X) fprintf(stderr, "%s:%d:%d error: %s\n", FileName.c_str(), Row, Col, X); assert(false);

/////////////////////////////////////////////////////////
/// Lexer
/////////////////////////////////////////////////////////

/// 获取token
static int LastChar = ' ';

static int getToken() {

    while(isspace(LastChar)) {
        if(LastChar == '\n') {
            Row++; Col = 0;
        }
        LastChar = fgetc(fp);
        Col++;
    }
        

    if(isalpha(LastChar)) { // identifier: [a-zA-Z][a-zA-Z0-9]*
        IdStr = LastChar;
        while(isalnum((LastChar = fgetc(fp)))) {
            IdStr += LastChar;
            Col++;
        }
            

        if (IdStr == "def")
            return tok_def;
        if (IdStr == "extern")
            return tok_extern;
        if (IdStr == "if")
            return tok_if;
        if (IdStr == "then")
            return tok_then;
        if (IdStr == "else")
            return tok_else; 
        if (IdStr == "for")
            return tok_for;
        if (IdStr == "in")
            return tok_in;
        if (IdStr == "while")
            return tok_while;
        if (IdStr == "return")    
            return tok_return;
        if (IdStr == "double")
            return tok_double;
        if (IdStr == "int")
            return tok_int;
        if (IdStr == "true")
            return tok_true;
        if (IdStr == "bool")
            return tok_bool;
        if (IdStr == "false")
            return tok_false;
        if (IdStr == "void")
            return tok_void;
        return tok_id;
    }

    if (isdigit(LastChar)) {
        std::string NumStr;
        bool isFloat = false;
        do {
            NumStr += LastChar;
            LastChar = fgetc(fp);
            Col++;
            if(LastChar == '.')
                isFloat = true;
        } while(isdigit(LastChar) || LastChar == '.');

        if(isFloat) {
            DoubleNumVal = strtod(NumStr.c_str(), nullptr);
            return tok_double_number;
        }

        IntNumVal = atoi(NumStr.c_str());
        return tok_int_number;
    }

    switch (LastChar)
    {
        case '+' : LastChar = fgetc(fp); Col++; return tok_plus;
        case '-' : LastChar = fgetc(fp); Col++; return tok_sub;
        case '*' : LastChar = fgetc(fp); Col++; return tok_mul;
        case '/' : LastChar = fgetc(fp); Col++; return tok_div;
        case '>' : {
            LastChar = fgetc(fp);
            Col++;
            if(LastChar == '=') {
                LastChar = fgetc(fp);
                Col++;
                return tok_gteq;
            }
            return tok_gt;
        }
        case '<' : {
            LastChar = fgetc(fp);
            Col++;
            if(LastChar == '=') {
                LastChar = fgetc(fp);
                Col++;
                return tok_lteq;
            }             
            return tok_lt;
        }
        case '=' : {
            LastChar = fgetc(fp);
            Col++;
            if(LastChar == '=') {
                LastChar = fgetc(fp);
                Col++;
                return tok_eq;
            }
            return tok_assign;
        }
        case '!' : {
            LastChar = fgetc(fp);
            Col++;
            if(LastChar == '=') {
                LastChar = fgetc(fp);
                Col++;
                return tok_neq;
            }
            return tok_not;
        }
    }

    if(LastChar == '#') {
        do {
            LastChar = fgetc(fp);
            Col++;
        }
        while(LastChar != EOF && LastChar != '\n');

        if(LastChar != EOF)
            return getToken();
    }

    if(LastChar == EOF)
        return tok_eof; 

    int ThisChar = LastChar;
    LastChar = fgetc(fp);
    Col++;
    return ThisChar;
}

void unitTest(const char* filepath) {
    openFile(filepath);
    int tok;
    tok = getToken();
    while(tok != tok_eof) {
        printf("Token value = %d, Row %d, Col %d\n", tok, Row, Col);
        tok = getToken();
    }
}

static int CurTok;
static int CurrSeek;
static int getNextToken() { return CurTok = getToken(); }

/// 前看N个token
std::vector<int> lookUp(int n) {
    std::vector<int> LookUpToks;
    int CurrentLoc = ftell(fp);
    int SavedChar = LastChar;
    unsigned SavedRow = Row, SavedCol = Col;
    int i = 0, tok;
    while (i < n && CurTok != tok_eof) {
        tok = getToken();
        LookUpToks.push_back(tok);
        i++;
    }
    fseek(fp, CurrentLoc, SEEK_SET);  
    LastChar = SavedChar;
    Row = SavedRow; Col = SavedCol;

    return LookUpToks;
}

/////////////////////////////////////////////////////////
/// Parser
/////////////////////////////////////////////////////////
static bool isFuncScope = false;

using SymbolTable = std::map<std::string, VarDeclAST*>;
static std::vector<SymbolTable> SymStack;
static std::map<std::string, ParamAST*> ParamMap;

void symbolTableAdd(const std::string& name, VarDeclAST *decl) {
    auto &symbolTab = SymStack[SymStack.size()-1];
    if(symbolTab.find(name) != symbolTab.end()) {
        LOG_ERROR("already define this symbol!");
    }
    symbolTab.insert({name, decl});
}

 VarDeclAST *symbolTableFind(const std::string& name) {
    auto it = SymStack.rbegin();
    auto end = SymStack.rend();

    while(it != end) {
        auto &tab = *it;
        if(tab.find(name) != tab.end()) {
            return tab[name];
        }
        it++;
    }

    return nullptr;
 }

 /// @brief  进入新的符号表
 void symbolTablePush() {
    SymStack.push_back(SymbolTable());
 }

 /// @brief 弹出顶层符号表
 void symbolTablePop() {
    SymStack.pop_back();
 }

 static std::map<std::string, FuncDefAST*> FuncDefMap;
 static std::map<std::string, FuncExternAST*> FuncExternMap;


ProgramAST *parseToGenerateAst(const char* filepath) {
    openFile(filepath);
    return parseProgram();
}

ProgramAST *parseProgram() {

    symbolTablePush();
    ProgramAST * program = new ProgramAST();
    /// deal with extern
    while(lookUp(1)[0] != tok_eof) {
        if(lookUp(1)[0] == tok_extern) {
            // eat extern
            getNextToken();
            if(lookUp(1)[0] != tok_id) {
                auto varExtern = parseVarExtern();
                program->addVarExtern(varExtern);
            }
            else {
                auto funcExtern = parseFuncExtern();
                program->addFuncExtern(funcExtern);
            }
        }
        else {
            if(lookUp(1)[0] == tok_def) {
                auto funcDef = parseFuncDef();
                program->addFuncDef(funcDef);
            }
            else {
                auto varDef = parseVarDef();
                program->addVarDef(varDef);
            }
        }
    }
    symbolTablePop();
    return program;
}

VarDefAST *parseVarDef() {
    // 1. type
    getNextToken();
    Token tk = (Token)CurTok;
    // 2. id
    VarDefAST *var = new VarDefAST(tk);
    while(lookUp(1)[0] != ';') {
        // eat id
        getNextToken();
        VarDeclAST *decl = new VarDeclAST();
        symbolTableAdd(IdStr, decl);
        decl->setVarName(IdStr);

        while(lookUp(1)[0] == '[') {
            // eat '['
            getNextToken();
            getNextToken(); 
            if(CurTok != tok_int_number) {
                LOG_ERROR("error array decl!")
            }
            decl->addArrayLen(IntNumVal);
            getNextToken();
            if(CurTok != ']') {
                LOG_ERROR("arry decl must end with ']'");
            }
        }

        var->addVarDecl(decl);

        if(lookUp(1)[0] == ',')
            getNextToken();     
    }

    // eat ';'
    getNextToken();

    return var;
}

const std::set<Token> TypeTokSet = {tok_void, tok_int, tok_double, tok_bool};

FuncDefAST *parseFuncDef() {
    // eat def
    getNextToken();
    // eat id
    getNextToken();

    if(FuncDefMap.find(IdStr) != FuncDefMap.end()) {
        LOG_ERROR("redefine this function");
    }

    FuncDefAST *funcDef = new FuncDefAST(IdStr, tok_void);
    FuncDefMap.insert({IdStr, funcDef});

    // eat '('
    getNextToken();
    if(CurTok != '(') {
        LOG_ERROR("func def not has '('");
    }

    // 解析参数
    while(lookUp(1)[0] != ')') {
        auto param = parseParam();
        funcDef->addParam(param);
        if(lookUp(1)[0] == ',')
            getNextToken();   
    }

    // eat ')'
    getNextToken();

    if(lookUp(1)[0] == ':') {
        // eat ':'
        getNextToken();
        // 解析返回值
        getNextToken();
        if(TypeTokSet.find((Token)CurTok) == TypeTokSet.end()) {
            LOG_ERROR("illegal ret type define");
        }
        funcDef->setRetTy((Token)CurTok);
    }

    funcDef->setStmt(parseBlockStmt());

    ParamMap.clear();

    return funcDef;
}

VarExternAST *parseVarExtern() {
    // eat type
    getNextToken();
    VarExternAST *extAst = new VarExternAST((Token)CurTok);

    while(lookUp(1)[0] != ';') {
        // eat id
        getNextToken();
        VarDeclAST *decl = new VarDeclAST();
        symbolTableAdd(IdStr, decl);
        decl->setVarName(IdStr);

        while(lookUp(1)[0] == '[') {
            // eat '['
            getNextToken();
            getNextToken(); 
            if(CurTok != tok_int_number) {
                LOG_ERROR("error array decl!")
            }
            decl->addArrayLen(IntNumVal);
            getNextToken();
            if(CurTok != ']') {
                LOG_ERROR("arry decl must end with ']'");
            }
        }

        extAst->addVarDecl(decl);

        if(lookUp(1)[0] == ',')
            getNextToken();     
    }

    // eat ';'
    getNextToken();

    return extAst;
}

FuncExternAST *parseFuncExtern() {
    // eat func id;
    getNextToken();

    if(FuncExternMap.find(IdStr) != FuncExternMap.end()) {
        LOG_ERROR("reextern this function")
    }

    FuncExternAST *funcExtern = new FuncExternAST(IdStr, tok_void);
    FuncExternMap.insert({IdStr, funcExtern});

    // eat '('
    getNextToken();
    if(CurTok != '(') {
        LOG_ERROR("func def not has '('");
    }

    // 解析参数
    while(lookUp(1)[0] != ')') {
        auto param = parseParam();
        funcExtern->addParam(param);
        if(lookUp(1)[0] == ',')
            getNextToken();
    }

    // eat ')'
    getNextToken();
    
    if(lookUp(1)[0] == ':') {
        // eat ':'
        getNextToken();
        // 解析返回值
        getNextToken();
        if(TypeTokSet.find((Token)CurTok) == TypeTokSet.end()) {
            LOG_ERROR("illegal ret type define");
        }
        funcExtern->setRetTy((Token)CurTok);
    }

    getNextToken();
    if(CurTok != ';') {
        LOG_ERROR("miss ';' in func extern")
    }

    return funcExtern;
}

ParamAST *parseParam() {
    // eat type
    getNextToken();
    Token type = (Token)(CurTok);
    // eat id
    getNextToken();
    ParamAST *param = new ParamAST(IdStr, type);
    ParamMap.insert({IdStr, param});
    while(lookUp(1)[0] == '[') {
        // eat '['
        getNextToken();

        if(lookUp(1)[0] == ']') {
            // eat ']'
            getNextToken();
            param->setIsArray();
            break;
        }

        // eat num;
        getNextToken();
        param->setIsArray();
        param->addArrayLen(IntNumVal);
        // eat ']'
        getNextToken();
        if(CurTok != ']') {
            LOG_ERROR("miss ']'");
        }
    }
    return param;
}

BlockStmtAST *parseBlockStmt() {

    symbolTablePush();
    // eat '{'
    getNextToken();
    if(CurTok != '{') {
        LOG_ERROR("miss '{' in block stmt");
    }

    BlockStmtAST *blockStmt = new BlockStmtAST();

    // 解析语句
    while(lookUp(1)[0] != '}') {
        blockStmt->addStatement(parseStatement());
        // getNextToken();
    }

    getNextToken();
    if(CurTok != '}') {
        LOG_ERROR("miss '}' in block stmt");
    }

    symbolTablePop();
    return blockStmt;
}

/// 前看相关的token
static Token PrevToks[10];
static IdRefAST *IdRef = nullptr;
static CallExprAST *Call = nullptr;
static IntExprAST *IntExpr = nullptr;
static DoubleExprAST *DoubleExpr = nullptr;
static BoolExprAST *BoolExpr = nullptr;

StatementAST *parseStatement() {
    switch (lookUp(1)[0])
    {
        case tok_int:
        case tok_double:
        case tok_bool:
            return (StatementAST*)parseVarDef();
        case tok_id: {
            if(lookUp(2)[1] == '(')
                return parseExprStmt();
            IdRef = (IdRefAST*)parseIdRef();
            if(lookUp(1)[0] == tok_assign) {
                return parseAssignStmt();
            }
            return parseExprStmt();
        }
        case tok_return: return parseReturnStmt();
        case tok_int_number:
        case tok_double_number: {
            return parseExprStmt();
        }
        case tok_if:
            return parseIfStmt();
        case tok_for:
            return parseForStmt();
        case tok_while:
            return parseWhileStmt();
        case '{':
            return parseBlockStmt();
        default:
            LOG_ERROR("unknown statement decl")
    }
}

StatementAST *parseReturnStmt() {
    // eat 'return'
    getNextToken();

    if(lookUp(1)[0] != ';') {
        ExprAST *expr = parseExpr();
        // eat ';'
        getNextToken();
        return (StatementAST*)(new ReturnStmtAST(expr));
    }

    // eat ';'
    getNextToken();
    return (StatementAST*)(new ReturnStmtAST());
}

StatementAST *parseIfStmt() {
    // eat if
    getNextToken();

    ExprAST *expr = parseExpr();

    // eat then
    getNextToken();

    StatementAST *stmt = parseStatement();

    IfStmtAST *ifStmt = new IfStmtAST(expr, stmt);

    // 判断else
    if(lookUp(1)[0] == tok_else) {
        // eat else
        getNextToken();
        StatementAST *elseStmt = parseStatement();
        ifStmt->setElseStatement(elseStmt);
    }

    return (StatementAST*)ifStmt;
}

StatementAST *parseForStmt() {
    // eat for
    getNextToken();

    ForStmtAST *forStmt = new ForStmtAST();

    if(lookUp(1)[0] == tok_id) {
        forStmt->setId((IdRefAST*)parseIdRef());
        // eat =
        getNextToken();
        forStmt->setAssignExpr(parseExpr());
    }

    // eat ','
    getNextToken();

    if(lookUp(1)[0] != ',') {
        forStmt->setCondExpr(parseExpr());
    }
        
    // eat ','
    getNextToken();

    if(lookUp(1)[0] != tok_in) {
        forStmt->setStepExpr(parseExpr());
    }
    
    // eat in
    getNextToken();

    forStmt->setStatement(parseStatement());

    return (StatementAST*)forStmt;
}

StatementAST *parseWhileStmt() {
    // eat while
    getNextToken();

    ExprAST *cond = parseExpr();

    WhileStmtAST *whileStmt = new WhileStmtAST(cond);

    // eat in
    getNextToken();

    whileStmt->setStatement(parseStatement());

    return (StatementAST*)whileStmt;
}

StatementAST *parseExprStmt() {
    ExprAST *expr = parseExpr();
    ExprStmtAST *exprStmt = new ExprStmtAST(expr); 
    
    getNextToken();
    if(CurTok != ';') {
        LOG_ERROR("miss ';' in expr stmt")
    }

    return dynamic_cast<StatementAST*>(exprStmt);
}


StatementAST *parseAssignStmt() {
    IdRefAST *idRef = IdRef;
    IdRef = nullptr;
    // eat =
    getNextToken();
    ExprAST *expr = parseExpr();
    AssignStmtAST *assignStmt = new AssignStmtAST(idRef, expr);

    // eat ;
    getNextToken();

    if(CurTok != ';') {
        LOG_ERROR("miss ';' in assign stmt")
    }

    return dynamic_cast<StatementAST*>(assignStmt);
}

static std::map<Token, Operator> TkToOpMap = {
    {tok_plus, Plus},
    {tok_sub, Sub},
    {tok_mul, Mul},
    {tok_div, Div},
    {tok_eq, Eq},
    {tok_neq, Neq},
    {tok_lt, Lt},
    {tok_lteq, LtEq},
    {tok_gt, Gt},
    {tok_gteq, GtEq},
    {tok_not, Not}
};

ExprAST *parseExpr() {
    return parseLogicExpr();
}

const static std::set<Token> LogicOpSet = {tok_lt, tok_lteq, tok_gt, tok_gteq, tok_eq, tok_neq};

ExprAST *parseLogicExpr() {
    ExprAST *left = parseBin1Expr();
    
    if(LogicOpSet.find((Token)lookUp(1)[0]) != LogicOpSet.end()) {
        getNextToken();
        Token op = (Token)CurTok;
        ExprAST *right = parseBin1Expr();
        BinExprAST *bin = new BinExprAST(TkToOpMap[op], left, right);
        return (ExprAST*)bin;
    }

    return left;
}

const static std::set<Token> Bin1OpSet = {tok_plus, tok_sub};

ExprAST *parseBin1Expr() {
    ExprAST *left = parseBin2Expr();

    if(Bin1OpSet.find((Token)lookUp(1)[0]) != Bin1OpSet.end()) {
        getNextToken();
        Token op = (Token)CurTok;
        ExprAST *right = parseBin2Expr();
        BinExprAST *bin = new BinExprAST(TkToOpMap[op], left, right);
        return (ExprAST*)bin;
    }

    return left;
}

const static std::set<Token> Bin2OpSet = {tok_mul, tok_div};

ExprAST *parseBin2Expr() {
    ExprAST *left = parseUnaryExpr();

    if(Bin2OpSet.find((Token)lookUp(1)[0]) != Bin2OpSet.end()) {
        getNextToken();
        Token op = (Token)CurTok;
        ExprAST *right = parseUnaryExpr();
        BinExprAST *bin = new BinExprAST(TkToOpMap[op], left, right);
        return (ExprAST*)bin;
    }

    return left;
}

ExprAST *parseUnaryExpr() {
    // eat value
    int Tok = lookUp(1)[0];
    if(Tok == tok_sub || Tok == tok_not) {
        getNextToken();
        Token op = (Token)CurTok;
        ExprAST *expr = parsePrimaryExpr();
        return (ExprAST*)(new UnaryExprAST(TkToOpMap[op], expr));
    }
    return parsePrimaryExpr();
}

ExprAST *parsePrimaryExpr() {
    if(IdRef) {
        ExprAST *expr = (ExprAST*)IdRef;
        IdRef = nullptr;
        return expr;
    }
    else {
        auto lkUp = lookUp(2); 
        if(lkUp[0] == '(') {
            // eat '('
            getNextToken();
            ExprAST *expr = parseExpr();
            getNextToken();
            if(CurTok != ')') {
                LOG_ERROR("miss ')' in primary expr")
            }
        }
        else if(lkUp[0] == tok_id && lkUp[1] == '('){
            return parseCall();
        }
        else if(lkUp[0] == tok_id) {
            return parseIdRef();
        }    
        getNextToken();
        switch (CurTok)
        {
            case tok_int_number : return (ExprAST *)(new IntExprAST(IntNumVal));
            case tok_double_number : return (ExprAST *)(new DoubleExprAST(DoubleNumVal));
            case tok_true : return (ExprAST *)(new BoolExprAST(true));
            case tok_false : return (ExprAST *)(new BoolExprAST(false));
            default: LOG_ERROR("parse expr error");
        }
    }
}

ExprAST *parseIdRef() {
    // eat id
    getNextToken();
    IdRefAST *idRef = new IdRefAST(IdStr, nullptr);
    VarDeclAST *decl = symbolTableFind(IdStr);
    if(!decl) {
        ParamAST *param = ParamMap[IdStr];
        if(!param) {
            LOG_ERROR("error ref not define or extern this var")
        }
        idRef->setParam(param);
    }
    else{
        idRef->setVarDecl(decl);
    }
    
    while(lookUp(1)[0] == '[') {
        // eat '['
        getNextToken();
        idRef->addArrayIndex(parseExpr());
        getNextToken();
        if(CurTok != ']') {
            LOG_ERROR("miss ']' in array indexes")
        }
    }
    return idRef;
}

ExprAST *parseCall() {
    // eat id
    getNextToken();
    CallExprAST *call = new CallExprAST(IdStr);
    if(FuncDefMap.find(IdStr) == FuncDefMap.end()) {
        if(FuncExternMap.find(IdStr) == FuncExternMap.end()) {
            LOG_ERROR("not define or extern this function")
        }
        else {
            call->setFuncExtern(FuncExternMap[IdStr]);
        }
    }
    else {
        call->setFuncDef(FuncDefMap[IdStr]);
    }
    

    // eat '('
    getNextToken();

    while(lookUp(1)[0] != ')') {
        call->addParam(parseExpr());
        if(lookUp(1)[0] == ',') {
            // eat ','
            getNextToken();
        }
    }

    // eat ')'
    getNextToken();

    return (ExprAST*)call;
}

