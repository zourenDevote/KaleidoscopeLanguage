
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
    if(LastChar == '"') {
        LiteralVal = "";
        getChar();
        // @TODO need to import check
        while(LastChar != '"') {
            LiteralVal += LastChar;
            getChar();
            // if(LastChar == '\n' || LastChar == ' ' || LastChar == '\r') {
            //     LOG_ERROR("Illigal character in import literal \'%c\'", LineInfo, LastChar);
            // }
        }
        getChar();
        return tok_literal;
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
    
    int i = 0, tok = getToken();
    while (i < n && tok != tok_eof) {
        LookUpToks.push_back((Token)tok);
        tok = getToken();
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
                        ProgAst->addChild(parseFuncExtern());
                    }
                    default:{
                        ProgAst->addChilds(parseVarExtern());
                    }
                }
                break;
            }
            case tok_import: {
                parseImportDecl();
                break;
            }
            case tok_def:{
                ProgAst->addChild(parseFuncDef());
                break;
            }
            default : {
                ProgAst->addChilds(parseVarDef());
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


std::vector<ASTBase*> GrammerParser::parseVarExtern()   {

    // eat extern
    getNextToken();

    bool isConst = false;
    if(TkParser->lookUp(1)[0] == tok_const) {
        getNextToken();
        isConst = true;
    }

    KType type = parseTypeDecl();
    std::vector<ASTBase*> externVarList;
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

            def->addChild(parseExpr());

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


ASTBase *GrammerParser::parseFuncExtern()  {
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



std::vector<ASTBase*> GrammerParser::parseVarDef() {
    
    bool isConst = false;
    if(TkParser->lookUp(1)[0] == tok_const) {
        // eat const
        getNextToken();
        isConst = true;
    }

    KType type = parseTypeDecl();
    std::vector<ASTBase*> externVarList;
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

            def->addChild(parseExpr());

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


ASTBase *GrammerParser::parseInitExpr()    {
    if(TkParser->lookUp(1)[0] == '{') {
        InitializedAST *init = new InitializedAST(TkParser->getCurLineNo());
        // eat '{'
        while(TkParser->lookUp(1)[0] != '}') {
            if(TkParser->lookUp(1)[0] == '{') {
                init->addChild(parseInitExpr());
            }
            else{
                init->addChild(parseExpr());
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


ASTBase *GrammerParser::parseFuncDef()     { 
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

    if(TkParser->lookUp(1)[0] != ';') {
        LOG_ERROR("missing ';'", TkParser->getCurLineNo())
    }

    funcDef->addChild(parseBlockStmt());

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


ASTBase *GrammerParser::parseStmt()        {

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
        default:{
            return parseExprStmt();
        }
    }
}


ASTBase *GrammerParser::parseBlockStmt()   {

    LineNo line = TkParser->getCurLineNo();
    // eat '{'
    getNextToken();

    BlockStmtAST *block = new BlockStmtAST(line);

    while(TkParser->lookUp(1)[0] != '}')  {
        block->addChild(parseStmt());
    }

    getNextToken();
    // eat '}'

    return block;
}


ASTBase *GrammerParser::parseIfStmt()      {

    LineNo line = TkParser->getCurLineNo();

    // eat 'if'
    getNextToken();

    ASTBase *expr = parseExpr();

    IfStmtAST *ifStmt = new IfStmtAST(line, expr);

    ifStmt->setStatement(parseStmt());

    if(TkParser->lookUp(1)[0] == tok_else) {
        // eat else
        getNextToken();
        ASTBase *elseStmt = parseStmt();
        ifStmt->setElse(elseStmt);
    }

    return ifStmt;
}


ASTBase *GrammerParser::parseExprStmt()    {

    ASTBase *expr = parseExpr();

    // eat ';'
    getNextToken();

    return expr;
}


ASTBase *GrammerParser::parseForStmt()     {

    LineNo line = TkParser->getCurLineNo();

    // eat 'for'
    getNextToken();

    ASTBase *expr1 = nullptr, *expr2 = nullptr, *expr3 = nullptr;

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


ASTBase *GrammerParser::parseWhileStmt()   {

    LineNo line = TkParser->getCurLineNo();

    // eat 'while'
    getNextToken();

    // eat '('
    getNextToken();

    ASTBase *expr = parseExpr();

    // eat ')'
    getNextToken();

    WhileStmtAST *whileStmt = new WhileStmtAST(line, expr);

    whileStmt->setStatement(parseStmt());

    return whileStmt;
}


ASTBase *GrammerParser::parseReturnStmt()  {

    ReturnStmtAST *returnStmt = new ReturnStmtAST(TkParser->getCurLineNo());

    getNextToken();

    if(TkParser->lookUp(1)[0] != ';') {
        returnStmt->addChild(parseExpr());
    }
    
    getNextToken();

    return returnStmt;

}


ASTBase *GrammerParser::parseBreakStmt()   {
    LineNo line = TkParser->getCurLineNo();
    getNextToken();
    getNextToken();
    return new BlockStmtAST(line);
}


ASTBase *GrammerParser::parseContinueStmt(){
    LineNo line = TkParser->getCurLineNo();
    getNextToken();
    getNextToken();
    return new ContinueStmtAST(line);
}

ASTBase *GrammerParser::parseSwitchStmt()  { return nullptr; }
ASTBase *GrammerParser::parseCaseStmt()    { return nullptr; }
ASTBase *GrammerParser::parseDefault()     { return nullptr; }

ASTBase *GrammerParser::parseExpr()        { return nullptr; }
ASTBase *GrammerParser::parseLogicExpr()   { return nullptr; }
ASTBase *GrammerParser::parseBitExpr()     { return nullptr; }
ASTBase *GrammerParser::parseCmpExpr()     { return nullptr; }
ASTBase *GrammerParser::parseBitMoveExpr() { return nullptr; }
ASTBase *GrammerParser::parseAddExpr()     { return nullptr; }
ASTBase *GrammerParser::parseMulExpr()     { return nullptr; }
ASTBase *GrammerParser::parseUnaryExpr()   { return nullptr; }
ASTBase *GrammerParser::parsePrimaryExpr() { return nullptr; }
ASTBase *GrammerParser::parseIdRef()       { return nullptr; }
ASTBase *GrammerParser::parseCallExpr()    { return nullptr; }
ASTBase *GrammerParser::parseConstExpr()   { return nullptr; }


/// -----------------------------------------------------




