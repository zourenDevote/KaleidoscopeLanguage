
#ifndef KALEIDOSCOPE_PARSER
#define KALEIDOSCOPE_PARSER

#include "token.h"
#include "ast.h"


/// -----------------------------------------------------
/// @brief Token parser class, this class defined to parse
/// sources file stream to Token. One TokenParser can deal
/// one sources file,  This object-oriented design, with 
/// fewer definitions of global variables, is designed to 
/// accommodate multithreaded compilation
/// -----------------------------------------------------
class TokenParser {
private:
    LineNo LineInfo;    
    double DoubleNumVal;
    long IntNumVal;
    std::string LiteralVal;
    std::string IdStr;
    int LastChar;
private:
    FILE *Handle;
public:
    Token getToken();
    void getChar();
public:
    explicit TokenParser(unsigned fileIndex);
    bool openSuccess();

    LineNo getCurLineNo() const { return LineInfo; }
    double getDoubleVal() const { return DoubleNumVal; }
    long   getIntVal()    const { return IntNumVal; }
    const std::string& getIdStr() const { return IdStr; }
    const std::string& getLiteral() const { return LiteralVal; }
    std::vector<Token> lookUp(unsigned i);
};
/// -----------------------------------------------------


/// -----------------------------------------------------
/// @brief 
/// -----------------------------------------------------
class GrammerParser {
private:
    ProgramAST *ProgAst;
    TokenParser *TkParser;
    Token CurTok;
public:
    explicit GrammerParser(ProgramAST *prog);
    void generateSrcToAst();
private:
    void getNextToken();
    void parseProgram();
    KType parseTypeDecl();
    std::vector<ASTBase*> parseVarExtern();
    ASTBase *parseFuncExtern();
    std::vector<ASTBase*> parseVarDef();
    ASTBase *parseInitExpr();
    ASTBase *parseFuncDef();
    ASTBase *parseImportDecl();
    ASTBase *parseParamList();
    ParamAST *parseParamDecl();
    ASTBase *parseStmt();
    ASTBase *parseBlockStmt();
    ASTBase *parseIfStmt();
    ASTBase *parseExprStmt();
    ASTBase *parseForStmt();
    ASTBase *parseWhileStmt();
    ASTBase *parseReturnStmt();
    ASTBase *parseBreakStmt();
    ASTBase *parseContinueStmt();
    ASTBase *parseSwitchStmt();
    ASTBase *parseCaseStmt();
    ASTBase *parseDefault();
    ASTBase *parseExpr();
    ASTBase *parseLogicExpr();
    ASTBase *parseBitExpr();
    ASTBase *parseCmpExpr();
    ASTBase *parseBitMoveExpr();
    ASTBase *parseAddExpr();
    ASTBase *parseMulExpr();
    ASTBase *parseUnaryExpr();
    ASTBase *parsePrimaryExpr();
    ASTBase *parseIdRef();
    ASTBase *parseCallExpr();
    ASTBase *parseConstExpr();

public:
    ProgramAST *getProg() { return ProgAst; }
};
/// -----------------------------------------------------


#endif
