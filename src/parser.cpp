
#include "parser.h"
#include "error.h"
#include "global_variable.h"


#include <string>
#include <cctype>
#include <cstdlib>
#include <cassert>
#include <vector>
#include <map>
#include <set>

namespace kale {

/// -----------------------------------------------------
/// @brief Code implication of class TokenParser
/// -----------------------------------------------------
TokenParser::TokenParser(unsigned fileIndex){
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

    if(LastChar == '#') {
        while (LastChar != EOF && LastChar != '\n') {
            getChar();
        }
        getChar();
    }

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
            LiteralVal += (char)LastChar;
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
        IsSigned = true;
        std::string NumStr = "";
        bool isFloat = false;
//        NumStr += LastChar;
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

        if(LastChar == 'U') {
            getChar();
            IsSigned = false;
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
/// @brief Code implication of class GrammarParser
/// -----------------------------------------------------
GrammarParser::GrammarParser(ProgramAST *prog) {
    ProgAst = prog;
    SymTabMap = {};
    GlobalVariableMap = {};
    TkParser = new TokenParser(prog->getLineNo()->FileIndex);
}

void GrammarParser::generateSrcToAst() {
    NodeStack.clear();
    parseProgram();

}

void GrammarParser::getNextToken() {
    CurTok = TkParser->getToken();
}

void GrammarParser::parseProgram() {

    assert(ProgAst && "Program ast can't be nullptr");
    NodeStack.push_back(ProgAst);
    while(TkParser->lookUp(1)[0] != tok_eof) {
        switch(TkParser->lookUp(1)[0]) {
            case tok_extern: {
                switch(TkParser->lookUp(2)[1]) {
                    case tok_id: {
                        ProgAst->addCompElem(parseFuncExtern());
                        break;
                    }
                    default: {
                        ProgAst->addCompElem(parseVarExtern());
                        break;
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
                ProgAst->addCompElem(parseVarDef());
                break;
            }
        }
    }
    NodeStack.pop_back();
}

DataTypeAST *GrammarParser::parseTypeDecl() {
    LineNo line = TkParser->getCurLineNo();
    KType datatype;
    getNextToken();
    switch (CurTok)
    {
    case tok_void:      { return nullptr;     }
    case tok_double:    { datatype = Double;break;   }
    case tok_float:     { datatype = Float;break;    }
    case tok_bool:      { datatype = Bool;break;     }
    case tok_char:      { datatype = Char;break;     }
    case tok_uchar:     { datatype = UChar;break;    }
    case tok_short:     { datatype = Short;break;    }
    case tok_ushort:    { datatype = UShort;break;   }
    case tok_int:       { datatype = Int;break;      }
    case tok_uint:      { datatype = Uint;break;     }
    case tok_long:      { datatype = Long;break;     }
    case tok_ulong:     { datatype = ULong;break;    }
    case tok_struct:    { datatype = Struct;break;   }
    default:
        LOG_ERROR("unsupport type declare", TkParser->getCurLineNo())
    }
    return new DataTypeAST(line, NodeStack.back(), datatype);
}


DataDeclAST *GrammarParser::parseVarExtern()   {

    LineNo line = TkParser->getCurLineNo();
    // eat extern
    getNextToken();

    DataDeclAST *Decl = new DataDeclAST(line, NodeStack.back());

    bool isConst = false;
    if(TkParser->lookUp(1)[0] == tok_const) {
        getNextToken();
        isConst = true;
    }

    DataTypeAST *type = parseTypeDecl();
    if(TkParser->lookUp(1)[0] != tok_id) {
        LOG_ERROR("without any data decl!", TkParser->getCurLineNo())
    }

    while(TkParser->lookUp(1)[0] != ';') {
        
        line = TkParser->getCurLineNo();
        // eat id
        getNextToken();

        if(CurTok != tok_id) {
            LOG_ERROR("Illegal variable extern declare", line)
        }

        auto *var = new VariableAST(line, Decl, TkParser->getIdStr());
        if(isConst) var->setIsConst();
        var->setIsExtern();
        var->setDataType(type);

        NodeStack.push_back(var);
        while(TkParser->lookUp(1)[0] == '[') {
            // eat '['
            getNextToken();
            
            line = TkParser->getCurLineNo();

            var->addDims(parseExpr());

            if(TkParser->lookUp(1)[0] != ']'){
                LOG_ERROR("not have dim declare", line)
            }

            getNextToken();
        }

        NodeStack.pop_back();
        Decl->addVarDecl(var);
        insertVariableToVarMap(var);
        if(TkParser->lookUp(1)[0] == ',') {
            getNextToken();
        }
    }

    // eat ';'
    getNextToken();

    if(CurTok != ';') {
        LOG_ERROR("missing ';'", TkParser->getCurLineNo())
    }

    return Decl;
}


FuncAST *GrammarParser::parseFuncExtern()  {
    LineNo line = TkParser->getCurLineNo();
    // eat extern
    getNextToken();
    // eat id
    getNextToken();
    FuncAST *funcExtern = new FuncAST(line, NodeStack.back(), TkParser->getIdStr());

    NodeStack.push_back(funcExtern);
    // parse params
    if(TkParser->lookUp(1)[0] != '(') {
        LOG_ERROR("missing '(' in function extern", TkParser->getCurLineNo());
    }

    // eat '('
    getNextToken();
    enterNewSymTab();
    while(TkParser->lookUp(1)[0] != ')') {
        funcExtern->addFuncParam(parseParamDecl());
        unsigned lookUp = TkParser->lookUp(1)[0];
        if(lookUp == ',') {
            // eat ','
            getNextToken();
        }
        else if(lookUp != ')') {
            LOG_ERROR("missing ')' in function decl", TkParser->getCurLineNo());
        }
    }
    leaveCurSymTab();

    // eat ')'
    getNextToken();

    if(TkParser->lookUp(1)[0] == ':') {
        getNextToken();
        funcExtern->setRetType(parseTypeDecl());
    }
    else {
        auto *retTy = new DataTypeAST(*funcExtern->getLineNo(), funcExtern, Void);
        funcExtern->setRetType(retTy);
    }

    if(TkParser->lookUp(1)[0] != ';') {
        LOG_ERROR("missing ';'", TkParser->getCurLineNo())
    }

    // eat ';'
    getNextToken();
    insertFunctionToFuncMap(funcExtern);
    NodeStack.push_back(funcExtern);
    return funcExtern;
}



DataDeclAST *GrammarParser::parseVarDef() {

    LineNo line = TkParser->getCurLineNo();
    DataDeclAST *Decl = new DataDeclAST(line, NodeStack.back());

    bool isConst = false;
    if(TkParser->lookUp(1)[0] == tok_const) {
        getNextToken();
        isConst = true;
    }
    DataTypeAST *type = parseTypeDecl();
    if(TkParser->lookUp(1)[0] != tok_id) {
        LOG_ERROR("without any data decl!", TkParser->getCurLineNo());
    }

    while(TkParser->lookUp(1)[0] != ';') {

        line = TkParser->getCurLineNo();
        // eat id
        getNextToken();

        if(CurTok != tok_id) {
            LOG_ERROR("Illegal variable extern declare", line);
        }

        VariableAST *var = new VariableAST(line, Decl, TkParser->getIdStr());
        if(isConst) var->setIsConst();
        var->setDataType(type);

        NodeStack.push_back(var);
        while(TkParser->lookUp(1)[0] == '[') {
            // eat '['
            getNextToken();

            line = TkParser->getCurLineNo();

            var->addDims(parseExpr());

            if(TkParser->lookUp(1)[0] != ']'){
                LOG_ERROR("not have dim declare", line)
            }

            getNextToken();
        }

        if(TkParser->lookUp(1)[0] == tok_assign) {
            getNextToken();
            var->setInitExpr(parseInitExpr());
        }

        NodeStack.pop_back();
        Decl->addVarDecl(var);
        insertVariableToVarMap(var);
        if(TkParser->lookUp(1)[0] == ',') {
            getNextToken();
        }
    }

    // eat ';'
    getNextToken();

    if(CurTok != ';') {
        LOG_ERROR("missing ';'", TkParser->getCurLineNo())
    }

    return Decl;
}


ExprAST *GrammarParser::parseInitExpr()    {
    if(TkParser->lookUp(1)[0] == '{') {
        InitializedAST *init = new InitializedAST(TkParser->getCurLineNo(), NodeStack.back());
        // eat '{'
        NodeStack.push_back(init);
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
        NodeStack.pop_back();

        // eat '}'
        getNextToken();
        return init;
    }
    return parseExpr();
}


FuncAST *GrammarParser::parseFuncDef()     {
    LineNo line = TkParser->getCurLineNo();

    // eat def
    getNextToken();

    // eat id
    getNextToken();

    FuncAST *funcDef = new FuncAST(line, NodeStack.back(), TkParser->getIdStr());
    enterNewSymTab();
    IsFuncScope = true;
    // parse params
    if(TkParser->lookUp(1)[0] != '(') {
        LOG_ERROR("missing '(' in function extern", TkParser->getCurLineNo());
    }

    NodeStack.push_back((ASTBase*)funcDef);
    // eat '('
    getNextToken();
    while(TkParser->lookUp(1)[0] != ')') {
        funcDef->addFuncParam(parseParamDecl());
        unsigned lookUp = TkParser->lookUp(1)[0];
        if(lookUp == ',') {
            // eat ','
            getNextToken();
        }
        else if(lookUp != ')') {
            LOG_ERROR("missing ')' in function decl", TkParser->getCurLineNo());
        }
    }

    // eat ')'
    getNextToken();

    if(TkParser->lookUp(1)[0] == ':') {
        getNextToken();
        funcDef->setRetType(parseTypeDecl());
    }
    else {
        auto *retTy = new DataTypeAST(*funcDef->getLineNo(), funcDef, Void);
        funcDef->setRetType(retTy);
    }
    insertFunctionToFuncMap(funcDef);
    funcDef->setBlockStmt(dynamic_cast<BlockStmtAST*>(parseBlockStmt()));
    NodeStack.pop_back();
    return funcDef;
}


ASTBase *GrammarParser::parseImportDecl()  {

    // eat import
    getNextToken();
    // eat literal
    getNextToken();
    // eat ';'
    getNextToken();

    return nullptr;
}


ASTBase *GrammarParser::parseParamList()   { return nullptr; }


ParamAST *GrammarParser::parseParamDecl()   {

    LineNo line = TkParser->getCurLineNo();
    ParamAST *param = new ParamAST(line, NodeStack.back(), nullptr);
    bool isConst = false;

    if(TkParser->lookUp(1)[0] == tok_const) {
        getNextToken();
        isConst = true;
    }

    // eat type
    DataTypeAST *datatype = parseTypeDecl();

    // eat id
    getNextToken();
    VariableAST *var = new VariableAST(line, param, TkParser->getIdStr());
    var->setDataType(datatype);
    NodeStack.push_back(var);
    while(TkParser->lookUp(1)[0] == '[') {
        // eat '['
        getNextToken();
        var->addDims(parseExpr());

        if(TkParser->lookUp(1)[0] != ']') {
            LOG_ERROR("missing ']' in param decl", TkParser->getCurLineNo());
        }
        // eat ']'
        getNextToken();
    }

    if(TkParser->lookUp(1)[0] == tok_assign) {
        var->setInitExpr(parseInitExpr());
    }

    NodeStack.pop_back();

    param->setId(var);
    insertVariableToVarMap(var);
    
    return param;
}


StatementAST *GrammarParser::parseStmt()        {

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


BlockStmtAST *GrammarParser::parseBlockStmt()   {
    if(!IsFuncScope) {
        enterNewSymTab();
    }
    else {
        IsFuncScope = false;
    }
    LineNo line = TkParser->getCurLineNo();
    // eat '{'
    getNextToken();

    BlockStmtAST *block = new BlockStmtAST(line, NodeStack.back());
    NodeStack.push_back(block);
    while(TkParser->lookUp(1)[0] != '}')  {
        if(StatementAST* stmt = parseStmt()) {
            block->addStmt(stmt);
        }
        else {
            block->addStmt(parseVarDef());
        }
    }
    NodeStack.pop_back();

    getNextToken();
    // eat '}'
    leaveCurSymTab();
    return block;
}


IfStmtAST *GrammarParser::parseIfStmt()      {

    LineNo line = TkParser->getCurLineNo();

    // eat 'if'
    getNextToken();

    IfStmtAST *ifStmt = new IfStmtAST(line, NodeStack.back(), nullptr);
    NodeStack.push_back(ifStmt);
    getNextToken();
    ifStmt->setCond(parseExpr());
    getNextToken();
    // eat then
    getNextToken();

    if(CurTok != tok_then) {
        LOG_ERROR("missing 'then' in if stmt", TkParser->getCurLineNo());
    }

    ifStmt->setStatement(parseStmt());

    if(TkParser->lookUp(1)[0] == tok_else) {
        // eat else
        getNextToken();
        ifStmt->setElse(parseStmt());
    }
    NodeStack.pop_back();

    return ifStmt;
}


ExprStmtAST *GrammarParser::parseExprStmt()    {
    LineNo line = TkParser->getCurLineNo();
    ExprStmtAST *exprStmt = new ExprStmtAST(line, NodeStack.back(), nullptr);
    NodeStack.push_back(exprStmt);
    exprStmt->setExpr(parseExpr());
    NodeStack.pop_back();
    // eat ';'
    getNextToken();
    return exprStmt;
}


ForStmtAST *GrammarParser::parseForStmt()     {

    LineNo line = TkParser->getCurLineNo();

    // eat 'for'
    getNextToken();

    ExprAST *expr1 = nullptr, *expr2 = nullptr, *expr3 = nullptr;
    ForStmtAST *forStmt = new ForStmtAST(line, NodeStack.back(), nullptr, nullptr, nullptr);
    NodeStack.push_back(forStmt);
    // eat '('
    getNextToken();

    if(TkParser->lookUp(1)[0] != ';') {
        expr1 = parseExpr();
    }
    getNextToken();

    if(TkParser->lookUp(1)[0] != ';') {
        expr2 = parseExpr();
    }
    getNextToken();

    if(TkParser->lookUp(1)[0] != ')') {
        expr3 = parseExpr();
    }

    forStmt->setExpr1(expr1);
    forStmt->setExpr2(expr2);
    forStmt->setExpr3(expr3);

    // eat ')'
    getNextToken();

    // eat 'in'
    getNextToken();


    forStmt->setStatement(parseStmt());
    NodeStack.pop_back();
    return forStmt;
}


WhileStmtAST *GrammarParser::parseWhileStmt()   {

    LineNo line = TkParser->getCurLineNo();

    // eat 'while'
    getNextToken();
    WhileStmtAST *whileStmt = new WhileStmtAST(line, NodeStack.back(), nullptr);
    NodeStack.push_back(whileStmt);
    // eat '('
    getNextToken();

    ExprAST *expr = parseExpr();
    whileStmt->setCond(expr);
    // eat ')'
    getNextToken();



    whileStmt->setStatement(parseStmt());
    NodeStack.pop_back();
    return whileStmt;
}


ReturnStmtAST *GrammarParser::parseReturnStmt()  {

    ReturnStmtAST *returnStmt = new ReturnStmtAST(TkParser->getCurLineNo(), NodeStack.back());

    getNextToken();

    NodeStack.push_back(returnStmt);
    if(TkParser->lookUp(1)[0] != ';') {
        returnStmt->setRetExpr(dynamic_cast<ExprAST*>(parseExpr()));
    }
    NodeStack.pop_back();
    
    getNextToken();

    return returnStmt;

}


BreakStmtAST *GrammarParser::parseBreakStmt()   {
    LineNo line = TkParser->getCurLineNo();
    getNextToken();
    getNextToken();
    return new BreakStmtAST(line, NodeStack.back());
}


ContinueStmtAST *GrammarParser::parseContinueStmt(){
    LineNo line = TkParser->getCurLineNo();
    getNextToken();
    getNextToken();
    return new ContinueStmtAST(line, NodeStack.back());
}


StatementAST *GrammarParser::parseSwitchStmt()  {assert(false && "Unsupport attribute"); return nullptr;}
StatementAST *GrammarParser::parseCaseStmt()    {assert(false && "Unsupport attribute"); return nullptr;}
StatementAST *GrammarParser::parseDefault()     {assert(false && "Unsupport attribute"); return nullptr;}


ExprAST *GrammarParser::parseExpr()        { return parseAssignExpr(); }

ExprAST *GrammarParser::parseAssignExpr() {
    LineNo line = TkParser->getCurLineNo();
    ExprAST *lhs = parseLogicExpr();
    BinaryExprAST *binExpr = nullptr;
    if(TkParser->lookUp(1)[0] == tok_assign) {
        getNextToken();
        if(lhs->getClassId() != IdRefId && lhs->getClassId() != IdIndexedRefId) {
            LOG_ERROR("error of left assign expr", line)
        }
        auto rhs = parseLogicExpr();
        binExpr = new BinaryExprAST(line, NodeStack.back(), Assign, lhs, rhs);
        lhs->setParent(binExpr);
        rhs->setParent(binExpr);
    }

    if(binExpr) return binExpr;
    return lhs;
}

ExprAST *GrammarParser::parseLogicExpr()   {
    
    static std::map<Token, Operator> LogicOpSet = { {tok_add, Add}, {tok_or, Or} }; 
    LineNo line = TkParser->getCurLineNo();
    ExprAST *lhs = parseBitExpr();
    BinaryExprAST *binExpr = nullptr;
    while(LogicOpSet.find(TkParser->lookUp(1)[0]) != LogicOpSet.end()) {
        // eat op
        getNextToken();
        Operator op = LogicOpSet[(Token)CurTok];
        ExprAST *rhs = parseBitExpr();
        binExpr = new BinaryExprAST(line, NodeStack.back(), op, lhs, rhs);
        lhs->setParent(binExpr);
        rhs->setParent(binExpr);
        line = TkParser->getCurLineNo();
        lhs = binExpr;
    }

    if(binExpr)
        binExpr;
    return lhs;

}


ExprAST *GrammarParser::parseBitExpr()     {

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
        binExpr = new BinaryExprAST(line, NodeStack.back(), op, lhs, rhs);
        lhs->setParent(binExpr);
        rhs->setParent(binExpr);
        line = TkParser->getCurLineNo();
        lhs = binExpr;
    }

    if(binExpr)
        return binExpr;
    return lhs;

}


ExprAST *GrammarParser::parseCmpExpr()     {

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
        binExpr = new BinaryExprAST(line, NodeStack.back(), op, lhs, rhs);
        lhs->setParent(binExpr);
        rhs->setParent(binExpr);
        line = TkParser->getCurLineNo();
        lhs = binExpr;
    }

    if(binExpr)
        return binExpr;
    return lhs;
}


ExprAST *GrammarParser::parseBitMoveExpr() {

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
        binExpr = new BinaryExprAST(line, NodeStack.back(), op, lhs, rhs);
        lhs->setParent(binExpr);
        rhs->setParent(binExpr);
        line = TkParser->getCurLineNo();
        lhs = binExpr;
    }

    if(binExpr)
        return binExpr;
    return lhs;

}


ExprAST *GrammarParser::parseAddExpr()     {

    static std::map<Token, Operator> AddOpSet = { {tok_add, Add}, {tok_sub, Sub} }; 
    LineNo line = TkParser->getCurLineNo();
    ExprAST *lhs = parseMulExpr();
    BinaryExprAST *binExpr = nullptr;
    while(AddOpSet.find(TkParser->lookUp(1)[0]) != AddOpSet.end()) {
        // eat op
        getNextToken();
        Operator op = AddOpSet[(Token)CurTok];
        ExprAST *rhs = parseMulExpr();
        binExpr = new BinaryExprAST(line, NodeStack.back(), op, lhs, rhs);
        lhs->setParent(binExpr);
        rhs->setParent(binExpr);
        line = TkParser->getCurLineNo();
        lhs = binExpr;
    }

    if(binExpr)
        return binExpr;
    return lhs;

}


ExprAST *GrammarParser::parseMulExpr()     {

    static std::map<Token, Operator> MulOpSet = { {tok_mul, Mul}, {tok_div, Div} }; 
    LineNo line = TkParser->getCurLineNo();
    ExprAST *lhs = parseUnaryExpr();
    BinaryExprAST *binExpr = nullptr;
    while(MulOpSet.find(TkParser->lookUp(1)[0]) != MulOpSet.end()) {
        // eat op
        getNextToken();
        Operator op = MulOpSet[(Token)CurTok];
        ExprAST *rhs = parseUnaryExpr();
        binExpr = new BinaryExprAST(line, NodeStack.back(), op, lhs, rhs);
        lhs->setParent(binExpr);
        rhs->setParent(binExpr);
        line = TkParser->getCurLineNo();
        lhs = binExpr;
    }

    if(binExpr)
        return binExpr;
    return lhs;

}


ExprAST *GrammarParser::parseUnaryExpr()   {
    static std::map<Token, Operator> UnaryOpSet = { {tok_add, Add}, {tok_sub, Sub}, {tok_not, Not} };
    LineNo line = TkParser->getCurLineNo();
    if(UnaryOpSet.find(TkParser->lookUp(1)[0]) != UnaryOpSet.end()) {
        getNextToken();
        Operator op = UnaryOpSet[(Token)CurTok];
        UnaryExprAST *unary = new UnaryExprAST(line, NodeStack.back(), op, nullptr);
        NodeStack.push_back(unary);
        unary->setUnaryExpr(parseUnaryExpr());
        NodeStack.pop_back();
        return unary;
    }
    return parsePrimaryExpr();
}


ExprAST *GrammarParser::parsePrimaryExpr() {
    
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

ExprAST *GrammarParser::parseIdRef()       {

    LineNo line = TkParser->getCurLineNo();
    getNextToken();

    if(TkParser->lookUp(1)[0] == '[') {
        auto *indexes = new IdIndexedRefAST(line, NodeStack.back(), TkParser->getIdStr());
        NodeStack.push_back(indexes);
        while(TkParser->lookUp(1)[0] == '[') {
            getNextToken();
            indexes->addIndex(parseExpr());
            getNextToken();
        }
        NodeStack.pop_back();
        if(auto var = getVariableNode(indexes->getIdName())) {
            indexes->setId(var);
        }
        else if(auto var = getVariableNodeFromGlobalMap(indexes->getIdName())) {
            indexes->setId(var);
        }
        else {
            LOG_ERROR("var not define!", line)
        }
        return indexes;
    }
    else {
        auto idref = new IdRefAST(line, NodeStack.back(), TkParser->getIdStr());
        if(auto var = getVariableNode(idref->getIdName())) {
            idref->setId(var);
        }
        else if(auto var = getVariableNodeFromGlobalMap(idref->getIdName())) {
            idref->setId(var);
        }
        else {
            LOG_ERROR("var not define!", line)
        }
        return idref;
    }

}


ExprAST *GrammarParser::parseCallExpr()    {

    LineNo line = TkParser->getCurLineNo();
    getNextToken();
    auto *callExpr = new CallExprAST(line, NodeStack.back(), TkParser->getIdStr());
    if(FuncAST *func = getFuncASTNode(callExpr->getName())) {
        callExpr->setFunction(func);
    }
    else {
        LOG_ERROR("not define or declare this function", line)
    }
    NodeStack.push_back(callExpr);
    // eat '('
    getNextToken();

    while(TkParser->lookUp(1)[0] != ')') {
        callExpr->addArg(parseExpr());
        if(TkParser->lookUp(1)[0] == ',')
            getNextToken();
        else
            break;
    }
    NodeStack.pop_back();

    // eat ')'
    getNextToken();

    return callExpr;
}


ExprAST *GrammarParser::parseConstExpr()   {
    LineNo line = TkParser->getCurLineNo();
    switch(TkParser->lookUp(1)[0]) {
        case tok_literal: {
            getNextToken();
            return new LiteralExprAST(line, NodeStack.back(), TkParser->getLiteral());
        }
        case tok_true: {
            getNextToken();
            return new NumberExprAST(line, NodeStack.back(), true);
        }
        case tok_false:
        {
            getNextToken();
            return new NumberExprAST(line, NodeStack.back(), false);
        }
        case tok_inumber:{
            getNextToken();
            NumberExprAST *number = new NumberExprAST(line, NodeStack.back(), TkParser->getIntVal());
            number->setIsSigned(TkParser->isSigned());
            return number;
        }
        case tok_fnumber:{
            getNextToken();
            return new NumberExprAST(line, NodeStack.back(), TkParser->getDoubleVal());
        }
        case tok_charlit:{
            getNextToken();
            return new NumberExprAST(line, NodeStack.back(), (char)TkParser->getIntVal());
        }
        default: {
            LOG_ERROR("error literal", line)
        }
    }
}


/// -----------------------------------------------------


/// -----------------------------------------------------
std::unordered_map<ProgramAST *, GrammarParser *> GrammarParser::ProgToGrammarParserMap = {};

GrammarParser *GrammarParser::getOrCreateGrammarParserByProg(ProgramAST *prog) {
    if(ProgToGrammarParserMap.find(prog) == ProgToGrammarParserMap.end()) {
        auto *parser = new GrammarParser(prog);
        ProgToGrammarParserMap.insert({prog, parser});
    }
    return ProgToGrammarParserMap[prog];
}
/// ------------------------------------------------------

FuncAST *GrammarParser::getFuncASTNode(const std::string &name) {
    if(FuncDefMap.find(name) != FuncDefMap.end())
        return FuncDefMap[name];
    for(auto *prog : ProgAst->getDependentProgs()){
        if(FuncAST *func = getOrCreateGrammarParserByProg(prog)->getFuncASTNode(name)){
            return func;
        }
    }
    return nullptr;
}

VariableAST *GrammarParser::getVariableNode(const std::string & name) {
    auto rbegin = SymTabMap.rbegin();
    auto rend = SymTabMap.rend();
    while(rbegin != rend) {
        auto& map = *rbegin;
        if(map.find(name) != map.end()) {
            return map[name];
        }
        rbegin++;
    }
    return nullptr;
}

VariableAST *GrammarParser::getVariableNodeFromGlobalMap(const std::string& name) {
    if(GlobalVariableMap.find(name) != GlobalVariableMap.end())
        return GlobalVariableMap[name];
    for(auto *prog : ProgAst->getDependentProgs()){
        auto parser = getOrCreateGrammarParserByProg(prog);
        if(VariableAST *var = parser->getVariableNodeFromOtherGlobalMap(name))
            return var;
    }
    return nullptr;
}

VariableAST *GrammarParser::getVariableNodeFromOtherGlobalMap(const std::string& name) {
    if(VariableAST *var = getVariableNodeFromGlobalMap(name)) {
        if(var->isStatic())
            return nullptr;
        return var;
    }
    return nullptr;
}

bool GrammarParser::insertFunctionToFuncMap(FuncAST *node) {
    if(FuncAST *func = getFuncASTNode(node->getFuncName()))
        return false;
    FuncDefMap.insert({node->getFuncName(), node});
    return true;
}

bool GrammarParser::insertVariableToVarMap(VariableAST *var) {
    if(SymTabMap.empty()) {
        if(VariableAST *var1 = getVariableNodeFromGlobalMap(var->getName()))
            return false;
        for(auto *prog : ProgAst->getDependentProgs()) {
            auto parser = getOrCreateGrammarParserByProg(prog);
            if(VariableAST *var1 = parser->getVariableNodeFromOtherGlobalMap(var->getName()))
                return false;
        }
        GlobalVariableMap.insert({var->getName(), var});
    }
    else {
        if(SymTabMap.back().find(var->getName()) != SymTabMap.back().end())
            return false;
        SymTabMap.back().insert({var->getName(), var});
    }
    return true;
}

void GrammarParser::enterNewSymTab() {
    SymTabMap.push_back({});
}

void GrammarParser::leaveCurSymTab() {
    SymTabMap.pop_back();
}

}


