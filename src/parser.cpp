
#include "parser.h"
#include "error.h"
#include "global_variable.h"


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

/// -----------------------------------------------------
/// @brief Code implication of class TokenParser
/// -----------------------------------------------------
TokenParser::TokenParser(unsigned fileIndex) {
    this->LineInfo = {fileIndex, 0, 0};
    this->DoubleNumVal = 0.0;
    this->IntNumVal = 0;
    this->LiteralVal = "";
    this->IdStr = "";
    this->LastChar = ' ';

    if(fileIndex < InputFileList.size()) {
        Handle = fopen(InputFileList[fileIndex].c_str(), "rt+");
    }
    else {
        Handle = nullptr;
    }
}

bool TokenParser::openSuccess() {
    return Handle != nullptr;
}


void TokenParser::getChar() {
    LastChar = fgetc(Handle);
    if(LastChar == '\n') {
        LineInfo.Row++; LineInfo.Col = 0;
    }
    else {
        LineInfo.Col++;
    }
}

static std::map<std::string, Token> KeyWordTkMap = {
    {"def", tok_def}, {"extern", tok_extern}, {"if", tok_if}, {"for", tok_for}, {"while", tok_while},
    {"else", tok_else}, {"then", tok_then}, {"in", tok_in}, {"return", tok_return}, {"continue", tok_continue},
    {"break", tok_break}, {"struct", tok_struct}, {"switch", tok_switch}, {"case", tok_case}, {"default", tok_default},
    {"true", tok_true}, {"false", tok_false}, {"void", tok_void}, {"bool", tok_bool}, {"char", tok_char}, {"uchar", tok_uchar},
    {"short", tok_short}, {"ushort", tok_ushort}, {"int", tok_int}, {"uint", tok_uint}, {"long", tok_long},
    {"ulong", tok_ulong}, {"float", tok_float}, {"double", tok_double}, {"import", tok_import}, {"const", tok_const}
};


Token TokenParser::getToken() {
    

    /// jump '\n', ' ', '\r'
    while(isspace(LastChar)) {
        getChar();
    }


    /// parse import "xxx.k"
    if(LastChar == '\"') {
        LiteralVal = "";
        getChar();
        // @TODO need to import check
        while(LastChar != '\"') {
            LiteralVal += LastChar;
            getChar();
            // if(LastChar == '\n' || LastChar == ' ' || LastChar == '\r') {
            //     LOG_ERROR("Illigal character in import literal \'%c\'", LineInfo, LastChar);
            // }
        }
        getChar();
        return tok_literal;
    }

    if(LastChar == '\'') {
        getChar();
        if(LastChar == '\'') {
            IntNumVal = '\0';
            getChar();
        }
        else{
            IntNumVal = LastChar;
            getChar();
            if(LastChar != '\''){
                LOG_ERROR("error character", getCurLineNo());
            }
            getChar();
        }
        return tok_charlit;
    }


    // identifier: [a-zA-Z][a-zA-Z0-9]*
    if(isalpha(LastChar)) { 
        IdStr = LastChar;
        getChar();
        while(isalnum((LastChar))) {
            IdStr += LastChar;
            getChar();
        }

        auto it = KeyWordTkMap.find(IdStr);

        if(it == KeyWordTkMap.end())
            return tok_id;
        return KeyWordTkMap[IdStr];
    } 

    if (isdigit(LastChar)) {
        std::string NumStr = "";
        bool isFloat = false;
        NumStr += LastChar;
        do {
            NumStr += LastChar;
            getChar();
            if(LastChar == '.')
                isFloat = true;
        } while(isdigit(LastChar) || LastChar == '.');

        if(isFloat) {
            DoubleNumVal = strtod(NumStr.c_str(), nullptr);
            return tok_fnumber;
        }

        IntNumVal = atol(NumStr.c_str());
        return tok_inumber;
    }

    switch(LastChar) {
        case '+': { getChar(); return tok_add; }
        case '-': { getChar(); return tok_sub; }
        case '*': { getChar(); return tok_mul; }
        case '/': { getChar(); return tok_div; }
        case '=': {
            getChar();
            if(LastChar == '=') {
                getChar(); 
                return tok_eq;
            }
            return tok_assign;
        }
        case '!':{
            getChar();
            if(LastChar == '=') {
                getChar();
                return tok_neq;
            }
            return tok_not;
        }
        case '.': { getChar(); return tok_dot; }
        case '>': { 
            getChar();
            if(LastChar == '=') {
                getChar();
                return tok_ge;
            }
            else if(LastChar == '>') {
                getChar();
                if(LastChar == '>') {
                    getChar();
                    return tok_urh;
                }
                return tok_rh;
            }
            return tok_gt;
        }
        case '<': {
            getChar();
            if(LastChar == '=') {
                getChar();
                return tok_le;
            }
            else if(LastChar == '<') {
                getChar();
                if(LastChar == '<') {
                    getChar();
                    return tok_ulh;
                }
                return tok_lh;
            }
            return tok_lt;
        }
        case '|': {
            getChar();
            if(LastChar == '|') {
                getChar();
                return tok_or;
            }
            return tok_bitor;
        }
        case '&':{
            getChar();
            if(LastChar == '&') {
                getChar();
                return tok_and;
            }
            return tok_bitand;
        }
        case '^': { getChar(); return tok_bitxor; }
    }

    if(LastChar == '#') {
        do {
            getChar();
        }
        while(LastChar != EOF && LastChar != '\n');
    }

    if(LastChar == EOF)
        return tok_eof; 

    int ThisChar = LastChar;
    getChar();

    return (Token)ThisChar;

}


std::vector<Token> TokenParser::lookUp(unsigned n) {
    std::vector<Token> LookUpToks;
    int CurrentLoc = ftell(Handle);
    int SavedChar = LastChar;
    LineNo preNo = LineInfo;
    
    int i = 0, tok;
    while (i < n) {
        tok = getToken();
        LookUpToks.push_back((Token)tok);
        if(tok == tok_eof)
            break;
        i++;
    }
    
    fseek(Handle, CurrentLoc, SEEK_SET);  
    
    LastChar = SavedChar;
    LineInfo = preNo;

    return LookUpToks;   
}

/// -----------------------------------------------------


/// -----------------------------------------------------
/// @brief Code implication of class GrammerParser
/// -----------------------------------------------------
GrammerParser::GrammerParser(ProgramAST *prog) {
    ProgAst = prog;
    TkParser = new TokenParser(prog->getLineNo()->FileIndex);
}

void GrammerParser::generateSrcToAst() {
    parseProgram();
}

void GrammerParser::getNextToken() {
    CurTok = TkParser->getToken();
}

void GrammerParser::parseProgram() {

    assert(ProgAst && "Program ast can't be nullptr");

    while(TkParser->lookUp(1)[0] != tok_eof) {
        switch(TkParser->lookUp(1)[0]) {
            case tok_extern: {
                switch(TkParser->lookUp(2)[1]) {
                    case tok_id: {
                        ProgAst->addCompElem(parseFuncExtern());
                    }
                    default:{
                        for(auto *vardef : parseVarExtern())
                            ProgAst->addCompElem(vardef);
                    }
                }
                break;
            }
            case tok_import: {
                parseImportDecl();
                break;
            }
            case tok_def:{
                ProgAst->addCompElem(parseFuncDef());
                break;
            }
            default : {
                for(auto *vardef : parseVarDef())
                    ProgAst->addCompElem(vardef);
                break;
            }
        }
    }
}

KType GrammerParser::parseTypeDecl() {
    getNextToken();
    switch (CurTok)
    {
    case tok_void:
        return Void;
    case tok_double:
        return Double;
    case tok_float:
        return Float;
    case tok_bool:
        return Bool;
    case tok_char:
        return Char;
    case tok_uchar:
        return UChar;
    case tok_short:
        return Short;
    case tok_ushort:
        return UShort;
    case tok_int:
        return Int;
    case tok_uint:
        return Uint;
    case tok_long:
        return Long;
    case tok_ulong:
        return ULong;
    case tok_struct:
        return Struct;
    default:
        LOG_ERROR("unsupport type declare", TkParser->getCurLineNo())
    }
}


std::vector<VarDefAST*> GrammerParser::parseVarExtern()   {

    // eat extern
    getNextToken();

    bool isConst = false;
    if(TkParser->lookUp(1)[0] == tok_const) {
        getNextToken();
        isConst = true;
    }

    KType type = parseTypeDecl();
    std::vector<VarDefAST*> externVarList;
    LineNo line;
    VarDefAST *def;
    while(TkParser->lookUp(1)[0] != ';') {
        
        line = TkParser->getCurLineNo();
        // eat id
        getNextToken();

        if(CurTok != tok_id) {
            LOG_ERROR("Illegal variable extern declare", line);
        }

        def = new VarDefAST(line, TkParser->getIdStr(), type);
        def->setIsConst(isConst);

        while(TkParser->lookUp(1)[0] == '[') {
            // eat '['
            getNextToken();
            
            line = TkParser->getCurLineNo();
            if(TkParser->lookUp(1)[0] != ']'){
                LOG_ERROR("not have dim declare", line)
            } 

            def->setInitExpr(parseExpr());

            line = TkParser->getCurLineNo();
            getNextToken();

            if(CurTok != ']') {
                LOG_ERROR("missing ']'", line)
            }

        }

        externVarList.push_back(def);

        if(TkParser->lookUp(1)[0] == ',') {
            getNextToken();
        }
    }

    // eat ';'
    getNextToken();

    if(CurTok != ';') {
        LOG_ERROR("missing ';'", TkParser->getCurLineNo())
    }

    return externVarList;
}


FuncAST *GrammerParser::parseFuncExtern()  {
    LineNo line = TkParser->getCurLineNo();

    // eat exteran
    getNextToken();

    // eat id
    getNextToken();

    FuncAST *funcExtern = new FuncAST(line, TkParser->getIdStr(), Void);

    // parse params
    if(TkParser->lookUp(1)[0] != '(') {
        LOG_ERROR("missing '(' in function extern", TkParser->getCurLineNo());
    }

    // eat '('
    getNextToken();
    while(TkParser->lookUp(1)[0] != ')') {
        funcExtern->addFuncParam(parseParamDecl());
        line = TkParser->getCurLineNo();
        if(TkParser->lookUp(1)[0] == ',') {
            // eat ','
            getNextToken();
        }
        else {
            LOG_ERROR("missing ')'", line)
        }
    }

    // eat ')'
    getNextToken();

    if(TkParser->lookUp(1)[0] == ':') {
        getNextToken();
        funcExtern->setRetType(parseTypeDecl());
    }

    if(TkParser->lookUp(1)[0] != ';') {
        LOG_ERROR("missing ';'", TkParser->getCurLineNo())
    }

    // eat ';'
    getNextToken();

    return funcExtern;
}



std::vector<VarDefAST*> GrammerParser::parseVarDef() {
    
    bool isConst = false;
    if(TkParser->lookUp(1)[0] == tok_const) {
        // eat const
        getNextToken();
        isConst = true;
    }

    KType type = parseTypeDecl();
    std::vector<VarDefAST*> externVarList;
    LineNo line;
    VarDefAST *def;
    while(TkParser->lookUp(1)[0] != ';') {
        
        line = TkParser->getCurLineNo();
        // eat id
        getNextToken();

        if(CurTok != tok_id) {
            LOG_ERROR("Illegal variable extern declare", line);
        }

        def = new VarDefAST(line, TkParser->getIdStr(), type);
        def->setIsConst(isConst);

        while(TkParser->lookUp(1)[0] == '[') {
            // eat '['
            getNextToken();
            
            line = TkParser->getCurLineNo();
            if(TkParser->lookUp(1)[0] != ']'){
                LOG_ERROR("not have dim declare", line)
            } 

            def->setInitExpr(parseExpr());

            line = TkParser->getCurLineNo();
            getNextToken();

            if(CurTok != ']') {
                LOG_ERROR("missing ']'", line)
            }

        }

        if(TkParser->lookUp(1)[0] == tok_assign) {
            // eat assign
            getNextToken();
            // 
            def->setInitExpr(parseInitExpr());
        }

        externVarList.push_back(def);

        if(TkParser->lookUp(1)[0] == ',') {
            getNextToken();
        }
    }

    // eat ';'
    getNextToken();

    if(CurTok != ';') {
        LOG_ERROR("missing ';'", TkParser->getCurLineNo())
    }

    return externVarList;

}


ExprAST *GrammerParser::parseInitExpr()    {
    if(TkParser->lookUp(1)[0] == '{') {
        InitializedAST *init = new InitializedAST(TkParser->getCurLineNo());
        // eat '{'
        while(TkParser->lookUp(1)[0] != '}') {
            if(TkParser->lookUp(1)[0] == '{') {
                init->setInitExpr(dynamic_cast<InitializedAST*>(parseInitExpr()));
            }
            else{
                init->setExpr(parseExpr());
            }

            if(TkParser->lookUp(1)[0] == ',')
                getNextToken();
        }

        // eat '}'
        getNextToken();

        return init;
    }
    return parseExpr();
}


FuncAST *GrammerParser::parseFuncDef()     {
    LineNo line = TkParser->getCurLineNo();

    // eat def
    getNextToken();

    // eat id
    getNextToken();

    FuncAST *funcDef = new FuncAST(line, TkParser->getIdStr(), Void);

    // parse params
    if(TkParser->lookUp(1)[0] != '(') {
        LOG_ERROR("missing '(' in function extern", TkParser->getCurLineNo());
    }

    // eat '('
    getNextToken();
    while(TkParser->lookUp(1)[0] != ')') {
        funcDef->addFuncParam(parseParamDecl());
        line = TkParser->getCurLineNo();
        if(TkParser->lookUp(1)[0] == ',') {
            // eat ','
            getNextToken();
        }
        else {
            LOG_ERROR("missing ')'", line)
        }
    }

    // eat ')'
    getNextToken();

    if(TkParser->lookUp(1)[0] == ':') {
        getNextToken();
        funcDef->setRetType(parseTypeDecl());
    }

    funcDef->setBlockStmt(dynamic_cast<BlockStmtAST*>(parseBlockStmt()));

    return funcDef;
}


ASTBase *GrammerParser::parseImportDecl()  {

    // eat import
    getNextToken();
    // eat literal
    getNextToken();
    // eat ';'
    getNextToken();

    return nullptr;
}


ASTBase *GrammerParser::parseParamList()   { return nullptr; }


ParamAST *GrammerParser::parseParamDecl()   {

    LineNo line = TkParser->getCurLineNo();
    bool isConst = false;

    if(TkParser->lookUp(1)[0] == tok_const) {
        getNextToken();
        isConst = true;
    }

    // eat type
    KType type = parseTypeDecl();

    // eat id
    getNextToken();

    ParamAST *param = new ParamAST(line, TkParser->getIdStr(), type);
    param->setIsConst(isConst);

    while(TkParser->lookUp(1)[0] == '[') {
        // eat '['
        getNextToken();

        if(TkParser->lookUp(1)[0] == ']') {
            getNextToken();
            param->arrayDimAdd();
            continue;
        }

        // eat number
        getNextToken();
        param->arrayDimAdd();
        param->addArrayDim(TkParser->getIntVal());

        // eat ']'
        getNextToken();
    }
    
    return param;
}


StatementAST *GrammerParser::parseStmt()        {

    int tk = TkParser->lookUp(1)[0];
    switch(tk) {
        case tok_return: {
            return parseReturnStmt();
        }
        case tok_continue: {
            return parseContinueStmt();
        }
        case tok_break:{
            return parseBreakStmt();
        }
        case tok_for:{
            return parseForStmt();
        }
        case tok_while:{
            return parseWhileStmt();
        }
        case tok_if:{
            return parseIfStmt();
        }
        case '{':{
            return parseBlockStmt();
        }
        case tok_switch:{
            return parseSwitchStmt();
        }
        case tok_bool:
        case tok_int:
        case tok_uint:
        case tok_char:
        case tok_uchar:
        case tok_short:
        case tok_ushort:
        case tok_long:
        case tok_ulong:
        case tok_double:
        case tok_float:
            return nullptr;
        default:{
            return parseExprStmt();
        }
    }
}


BlockStmtAST *GrammerParser::parseBlockStmt()   {

    LineNo line = TkParser->getCurLineNo();
    // eat '{'
    getNextToken();

    BlockStmtAST *block = new BlockStmtAST(line);

    while(TkParser->lookUp(1)[0] != '}')  {
        if(StatementAST* stmt = parseStmt()) {
            block->addStmt(stmt);
        }
        else {
            for(auto *def : parseVarDef()) {
                block->addStmt(def);
            }
        }
    }

    getNextToken();
    // eat '}'

    return block;
}


IfStmtAST *GrammerParser::parseIfStmt()      {

    LineNo line = TkParser->getCurLineNo();

    // eat 'if'
    getNextToken();

    IfStmtAST *ifStmt = new IfStmtAST(line, parseExpr());

    ifStmt->setStatement(parseStmt());

    if(TkParser->lookUp(1)[0] == tok_else) {
        // eat else
        getNextToken();
        ifStmt->setElse(parseStmt());
    }

    return ifStmt;
}


ExprStmtAST *GrammerParser::parseExprStmt()    {

    LineNo line = TkParser->getCurLineNo();
    ExprStmtAST *exprStmt = new ExprStmtAST(line, parseExpr());

    // eat ';'
    getNextToken();

    return exprStmt;

}


ForStmtAST *GrammerParser::parseForStmt()     {

    LineNo line = TkParser->getCurLineNo();

    // eat 'for'
    getNextToken();

    ExprAST *expr1 = nullptr, *expr2 = nullptr, *expr3 = nullptr;

    // eat '('
    getNextToken();

    if(TkParser->lookUp(1)[0] != ',') {
        expr1 = parseExpr();
    }

    getNextToken();

    if(TkParser->lookUp(1)[0] != ',') {
        expr2 = parseExpr();
    }

    getNextToken();

    if(TkParser->lookUp(1)[0] != tok_in) {
        expr3 = parseExpr();
    }

    // eat ')'
    getNextToken();

    // eat 'in'
    getNextToken();

    ForStmtAST *forStmt = new ForStmtAST(line, expr1, expr2, expr3);

    forStmt->setStatement(parseStmt());

    return forStmt;
}


WhileStmtAST *GrammerParser::parseWhileStmt()   {

    LineNo line = TkParser->getCurLineNo();

    // eat 'while'
    getNextToken();

    // eat '('
    getNextToken();

    ExprAST *expr = parseExpr();

    // eat ')'
    getNextToken();

    WhileStmtAST *whileStmt = new WhileStmtAST(line, expr);

    whileStmt->setStatement(parseStmt());

    return whileStmt;
}


ReturnStmtAST *GrammerParser::parseReturnStmt()  {

    ReturnStmtAST *returnStmt = new ReturnStmtAST(TkParser->getCurLineNo());

    getNextToken();

    if(TkParser->lookUp(1)[0] != ';') {
        returnStmt->setRetExpr(dynamic_cast<ExprAST*>(parseExpr()));
    }
    
    getNextToken();

    return returnStmt;

}


BreakStmtAST *GrammerParser::parseBreakStmt()   {
    LineNo line = TkParser->getCurLineNo();
    getNextToken();
    getNextToken();
    return new BreakStmtAST(line);
}


ContinueStmtAST *GrammerParser::parseContinueStmt(){
    LineNo line = TkParser->getCurLineNo();
    getNextToken();
    getNextToken();
    return new ContinueStmtAST(line);
}


StatementAST *GrammerParser::parseSwitchStmt()  {assert(false && "Unsupport attribute"); return nullptr;}
StatementAST *GrammerParser::parseCaseStmt()    {assert(false && "Unsupport attribute"); return nullptr;}
StatementAST *GrammerParser::parseDefault()     {assert(false && "Unsupport attribute"); return nullptr;}


ExprAST *GrammerParser::parseExpr()        { return parseLogicExpr(); }

ExprAST *GrammerParser::parseLogicExpr()   {
    
    static std::map<Token, Operator> LogicOpSet = { {tok_add, Add}, {tok_or, Or} }; 
    LineNo line = TkParser->getCurLineNo();
    ExprAST *lhs = parseBitExpr();
    BinaryExprAST *binExpr = nullptr;
    while(LogicOpSet.find(TkParser->lookUp(1)[0]) != LogicOpSet.end()) {
        // eat op
        getNextToken();
        Operator op = LogicOpSet[(Token)CurTok];
        ExprAST *rhs = parseBitExpr();
        binExpr = new BinaryExprAST(line, op, lhs, rhs);
        line = TkParser->getCurLineNo();
        lhs = binExpr;
    }

    if(binExpr)
        binExpr;
    return lhs;

}


ExprAST *GrammerParser::parseBitExpr()     {

    static std::map<Token, Operator> BitOpSet = { {tok_bitxor, BitXor}, {tok_bitor, BitOr},
        {tok_bitand, BitAnd}}; 
    LineNo line = TkParser->getCurLineNo();
    ExprAST *lhs = parseCmpExpr();
    BinaryExprAST *binExpr = nullptr;
    while(BitOpSet.find(TkParser->lookUp(1)[0]) != BitOpSet.end()) {
        // eat op
        getNextToken();
        Operator op = BitOpSet[(Token)CurTok];
        ExprAST *rhs = parseCmpExpr();
        binExpr = new BinaryExprAST(line, op, lhs, rhs);
        line = TkParser->getCurLineNo();
        lhs = binExpr;
    }

    if(binExpr)
        return binExpr;
    return lhs;

}


ExprAST *GrammerParser::parseCmpExpr()     {

    static std::map<Token, Operator> CmpOpSet = { {tok_gt, Gt}, {tok_ge, Ge},
        {tok_lt, Lt}, {tok_le, Le}, {tok_eq, Eq}, {tok_neq, Neq}}; 
    LineNo line = TkParser->getCurLineNo();
    ExprAST *lhs = parseBitMoveExpr();
    BinaryExprAST *binExpr = nullptr;
    while(CmpOpSet.find(TkParser->lookUp(1)[0]) != CmpOpSet.end()) {
        // eat op
        getNextToken();
        Operator op = CmpOpSet[(Token)CurTok];
        ExprAST *rhs = parseBitMoveExpr();
        binExpr = new BinaryExprAST(line, op, lhs, rhs);
        line = TkParser->getCurLineNo();
        lhs = binExpr;
    }

    if(binExpr)
        return binExpr;
    return lhs;
}


ExprAST *GrammerParser::parseBitMoveExpr() {

    static std::map<Token, Operator> BitMoveOpSet = { {tok_lh, Lsft}, {tok_ulh, Lusft},
        {tok_rh, Rsft}, {tok_urh, Rusft}}; 
    LineNo line = TkParser->getCurLineNo();
    ExprAST *lhs = parseAddExpr();
    BinaryExprAST *binExpr = nullptr;
    while(BitMoveOpSet.find(TkParser->lookUp(1)[0]) != BitMoveOpSet.end()) {
        // eat op
        getNextToken();
        Operator op = BitMoveOpSet[(Token)CurTok];
        ExprAST *rhs = parseAddExpr();
        binExpr = new BinaryExprAST(line, op, lhs, rhs);
        line = TkParser->getCurLineNo();
        lhs = binExpr;
    }

    if(binExpr)
        return binExpr;
    return lhs;

}


ExprAST *GrammerParser::parseAddExpr()     {

    static std::map<Token, Operator> AddOpSet = { {tok_add, Add}, {tok_sub, Sub} }; 
    LineNo line = TkParser->getCurLineNo();
    ExprAST *lhs = parseMulExpr();
    BinaryExprAST *binExpr = nullptr;
    while(AddOpSet.find(TkParser->lookUp(1)[0]) != AddOpSet.end()) {
        // eat op
        getNextToken();
        Operator op = AddOpSet[(Token)CurTok];
        ExprAST *rhs = parseMulExpr();
        binExpr = new BinaryExprAST(line, op, lhs, rhs);
        line = TkParser->getCurLineNo();
        lhs = binExpr;
    }

    if(binExpr)
        return binExpr;
    return lhs;

}


ExprAST *GrammerParser::parseMulExpr()     {

    static std::map<Token, Operator> MulOpSet = { {tok_mul, Mul}, {tok_div, Div} }; 
    LineNo line = TkParser->getCurLineNo();
    ExprAST *lhs = parseUnaryExpr();
    BinaryExprAST *binExpr = nullptr;
    while(MulOpSet.find(TkParser->lookUp(1)[0]) != MulOpSet.end()) {
        // eat op
        getNextToken();
        Operator op = MulOpSet[(Token)CurTok];
        ExprAST *rhs = parseUnaryExpr();
        binExpr = new BinaryExprAST(line, op, lhs, rhs);
        line = TkParser->getCurLineNo();
        lhs = binExpr;
    }

    if(binExpr)
        return binExpr;
    return lhs;

}


ExprAST *GrammerParser::parseUnaryExpr()   {
    static std::map<Token, Operator> UnaryOpSet = { {tok_add, Add}, {tok_sub, Sub}, {tok_not, Not} };
    LineNo line = TkParser->getCurLineNo();
    if(UnaryOpSet.find(TkParser->lookUp(1)[0]) != UnaryOpSet.end()) {
        getNextToken();
        Operator op = UnaryOpSet[(Token)CurTok];
        return new UnaryExprAST(line, op, parseUnaryExpr());
    }
    return parsePrimaryExpr();
}


ExprAST *GrammerParser::parsePrimaryExpr() {
    
    switch (TkParser->lookUp(1)[0]) {
        case '(': {
            getNextToken();
            ExprAST *expr = parseExpr();
            getNextToken();
            return expr;
        }
        case tok_id: {
            if(TkParser->lookUp(2)[1] == '(')
                return parseCallExpr();
            return parseIdRef();
        }   
        default: {
            return parseConstExpr();
        }
    }
 
}


ExprAST *GrammerParser::parseIdRef()       {

    LineNo line = TkParser->getCurLineNo();
    getNextToken();

    if(TkParser->lookUp(1)[0] == '[') {
        IdIndexedRefAST *indexes = new IdIndexedRefAST(line, TkParser->getIdStr());
        while(TkParser->lookUp(1)[0] == '[') {
            getNextToken();
            indexes->addIndex(parseExpr());
            getNextToken();
        }
        return indexes;
    }
    else {
        return new IdRefAST(line, TkParser->getIdStr());
    }

}


ExprAST *GrammerParser::parseCallExpr()    {

    LineNo line = TkParser->getCurLineNo();
    getNextToken();
    CallExprAST *callExpr = new CallExprAST(line, TkParser->getIdStr());

    // eat '('
    getNextToken();

    while(TkParser->lookUp(1)[0] != ')') {
        callExpr->addArg(parseExpr());
        if(TkParser->lookUp(1)[0] == ',')
            getNextToken();
        else
            break;
    }

    // eat ')'
    getNextToken();

    return callExpr;
}


ExprAST *GrammerParser::parseConstExpr()   {
    LineNo line = TkParser->getCurLineNo();
    switch(TkParser->lookUp(1)[0]) {
        case tok_literal: {
            getNextToken();
            return new LiteralExprAST(line, TkParser->getLiteral());
        }
        case tok_true: {
            getNextToken();
            return new NumberExprAST(line, true);
        }
        case tok_false:
        {
            getNextToken();
            return new NumberExprAST(line, false);
        }
        case tok_inumber:{
            getNextToken();
            return new NumberExprAST(line, TkParser->getIntVal());
        }
        case tok_fnumber:{
            getNextToken();
            return new NumberExprAST(line, TkParser->getDoubleVal());
        }
        case tok_charlit:{
            getNextToken();
            return new NumberExprAST(line, (char)TkParser->getIntVal());
        }
        default: {
            LOG_ERROR("error literal", line);
        }
    }
}


/// -----------------------------------------------------




