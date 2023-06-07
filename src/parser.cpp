
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
    {"true", tok_true}, {"false", tok_false}, {"bool", tok_bool}, {"char", tok_char}, {"uchar", tok_uchar},
    {"short", tok_short}, {"ushort", tok_ushort}, {"int", tok_int}, {"uint", tok_uint}, {"long", tok_long},
    {"ulong", tok_ulong}, {"float", tok_float}, {"double", tok_double}, {"import", tok_import}
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

}

Token GrammerParser::getNextToken() {
    CurTok = getNextToken();
}

void GrammerParser::parseProgram() {

}

KType GrammerParser::parseTypeDecl() {
    return (KType)0;
}

ASTBase *GrammerParser::parseExternDef()   { return nullptr; }
ASTBase *GrammerParser::parseVarExtern()   { return nullptr; }
ASTBase *GrammerParser::parseFuncExtern()  { return nullptr; }
ASTBase *GrammerParser::parseVarDef()      { return nullptr; }
ASTBase *GrammerParser::parseInitExpr()    { return nullptr; }
ASTBase *GrammerParser::parseFuncDef()     { return nullptr; }
ASTBase *GrammerParser::parseImportDecl()  { return nullptr; }
ASTBase *GrammerParser::parseParamList()   { return nullptr; }
ASTBase *GrammerParser::parseParamDecl()   { return nullptr; }
ASTBase *GrammerParser::parseStmt()        { return nullptr; }
ASTBase *GrammerParser::parseBlockStmt()   { return nullptr; }
ASTBase *GrammerParser::parseIfStmt()      { return nullptr; }
ASTBase *GrammerParser::parseExprStmt()    { return nullptr; }
ASTBase *GrammerParser::parseForStmt()     { return nullptr; }
ASTBase *GrammerParser::parseWhileStmt()   { return nullptr; }
ASTBase *GrammerParser::parseReturnStmt()  { return nullptr; }
ASTBase *GrammerParser::parseBreakStmt()   { return nullptr; }
ASTBase *GrammerParser::parseContinueStmt(){ return nullptr; }
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




