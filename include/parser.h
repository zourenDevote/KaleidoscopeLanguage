
#ifndef KALEIDOSCOPE_PARSER
#define KALEIDOSCOPE_PARSER

#include "token.h"
#include "ast.h"

void unitTest(const char* filepath);

/// ----------------------------------------------------
/// This function is the top function of the parser
/// 
/// ----------------------------------------------------
ProgramAST *parseToGenerateAst(const char* filepath);


ProgramAST *parseProgram();

VarDefAST *parseVarDef();

FuncDefAST *parseFuncDef();

VarExternAST *parseVarExtern();

FuncExternAST *parseFuncExtern();

ParamAST *parseParam();

BlockStmtAST *parseBlockStmt();

StatementAST *parseStatement();

StatementAST *parseIfStmt();

StatementAST *parseForStmt();

StatementAST *parseWhileStmt();

StatementAST *parseExprStmt();

StatementAST *parseAssignStmt();

StatementAST *parseReturnStmt();

ExprAST *parseExpr();

ExprAST *parseLogicExpr();

ExprAST *parseEquExpr();

ExprAST *parseBin1Expr();

ExprAST *parseBin2Expr();

ExprAST *parseUnaryExpr();

ExprAST *parsePrimaryExpr();

ExprAST *parseIdRef();

ExprAST *parseCall();

#endif
