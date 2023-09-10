#include "ast.h"
#include <cassert>

namespace kale{

/// ----------------------------------------------------------
/// ASTBase code
ASTBase::ASTBase(const LineNo& lineNo, ASTBase *parent) : LineMsg(lineNo),
                                         Parent(parent),
                                         Program(nullptr){}
                                        

void ASTBase::setParent(ASTBase *parent) {
    this->Parent = parent;
}

void ASTBase::setLineNo(const LineNo& lineNo) {
    this->LineMsg = lineNo;
}

void ASTBase::setProgram(ProgramAST *prog) {
    this->Program = prog;
}


/// ----------------------------------------------------------

/// ----------------------------------------------------------
/// ProgramAST define code
ProgramAST::ProgramAST(const LineNo &lineNo, ASTBase *parent) : ASTBase(lineNo, parent) {
}
/// ----------------------------------------------------------

/// ----------------------------------------------------------
/// ParamAST define code
ParamAST::ParamAST(const LineNo &lineNo, ASTBase *parent, VariableAST *id) : ASTBase(lineNo, parent), Id(id) {

}
/// ----------------------------------------------------------

/// ----------------------------------------------------------
/// FuncAST define code
FuncAST::FuncAST(const LineNo &lineNo, ASTBase *parent, const std::string& funcName) : ASTBase(lineNo, parent) {
    this->FuncName = funcName;
    this->RetType = nullptr;
    this->BlockStmt = nullptr;
}
/// ----------------------------------------------------------

/// ----------------------------------------------------------
/// InitializedAST define code
InitializedAST::InitializedAST(const LineNo &lineNo, ASTBase *parent) : ExprAST(lineNo, parent) {}
/// ----------------------------------------------------------

/// ----------------------------------------------------------
/// StructDefAST define code
StructDefAST::StructDefAST(const LineNo &lineNo, ASTBase *parent) : ASTBase(lineNo, parent) {}
/// ----------------------------------------------------------

/// ----------------------------------------------------------
/// StatementAST define code
StatementAST::StatementAST(const LineNo& lineNo, ASTBase *parent) : ASTBase(lineNo, parent) {}
/// ----------------------------------------------------------

/// ----------------------------------------------------------
/// ExprStmtAST define code
ExprStmtAST::ExprStmtAST(const LineNo &lineNo, ASTBase *parent, ExprAST *expr) : StatementAST(lineNo, parent) {
    Expr = expr;
}
/// ----------------------------------------------------------

/// ----------------------------------------------------------
/// DataTypeAST define code
DataTypeAST::DataTypeAST(const LineNo& lineNo, ASTBase *parent, KType ty) : ASTBase(lineNo, parent), DataType(ty) {}
/// ----------------------------------------------------------

/// ----------------------------------------------------------
/// IdDefAST define code
IdDefAST::IdDefAST(const LineNo& lineNo, ASTBase *parent, const std::string &name) : ASTBase(lineNo, parent), Name(name){}
/// ----------------------------------------------------------

/// ----------------------------------------------------------
/// VariableAST define code
VariableAST::VariableAST(const LineNo& lineNo, ASTBase *parent, const std::string &name) : IdDefAST(lineNo, parent, name) {
    VarFlag = 0;
}
/// ----------------------------------------------------------

/// ----------------------------------------------------------
/// DataDeclAST define code
DataDeclAST::DataDeclAST(const LineNo &lineNo, ASTBase *parent) : StatementAST(lineNo, parent) {

}   
/// ----------------------------------------------------------

/// ----------------------------------------------------------
/// BlockStmtAST define code
BlockStmtAST::BlockStmtAST(const LineNo &lineNo, ASTBase *parent) : StatementAST(lineNo, parent){
}
/// ----------------------------------------------------------

/// ----------------------------------------------------------
/// ReturnStmtAST define code
ReturnStmtAST::ReturnStmtAST(const LineNo &lineNo, ASTBase *parent) : StatementAST(lineNo, parent){
    RetExpr = nullptr;
}
/// ----------------------------------------------------------

/// ----------------------------------------------------------
/// BreakStmtAST define code
BreakStmtAST::BreakStmtAST(const LineNo &lineNo, ASTBase *parent) : StatementAST(lineNo, parent){
}
/// ----------------------------------------------------------

/// ----------------------------------------------------------
/// ContinueStmtAST define code
ContinueStmtAST::ContinueStmtAST(const LineNo &lineNo, ASTBase *parent) : StatementAST(lineNo, parent){
}
/// ----------------------------------------------------------

/// ----------------------------------------------------------
/// ForStmtAST define code
ForStmtAST::ForStmtAST(const LineNo &lineNo, ASTBase *parent, ExprAST *expr1, ExprAST *expr2, ExprAST *expr3) : StatementAST(lineNo, parent) {
    this->Expr1 = expr1;
    this->Expr2 = expr2;
    this->Expr3 = expr3;
    this->Stmt = nullptr;
}

ForStmtAST::ForStmtAST(const LineNo &lineNo, ASTBase *parent) : StatementAST(lineNo, parent) {
    this->Expr1 = nullptr;
    this->Expr2 = nullptr;
    this->Expr3 = nullptr;
    this->Stmt = nullptr;
}
/// ----------------------------------------------------------

/// ----------------------------------------------------------
/// WhileStmtAST define code
WhileStmtAST::WhileStmtAST(const LineNo &lineNo, ASTBase *parent, ExprAST *cond) : StatementAST(lineNo, parent) {
    this->Cond = cond;
    this->Stmt = nullptr;
}
/// ----------------------------------------------------------

/// ----------------------------------------------------------
/// IfStmtAST define code
IfStmtAST::IfStmtAST(const LineNo &lineNo, ASTBase *parent, ExprAST *cond) : StatementAST(lineNo, parent) {
    this->Cond = cond;
    this->Stmt = nullptr;
    this->Else = nullptr;
}
/// ----------------------------------------------------------

/// ----------------------------------------------------------
/// ExprAST define code
ExprAST::ExprAST(const LineNo &lineNo, ASTBase *parent) : ASTBase(lineNo, parent) {}
/// ----------------------------------------------------------

/// ----------------------------------------------------------
/// BinaryExprAST define code
BinaryExprAST::BinaryExprAST(const LineNo &lineNo, ASTBase *parent, Operator op, ExprAST *lhs, ExprAST *rhs) : ExprAST(lineNo, parent) {
    this->Op = op;
    assert(lhs && "Lhs can not be null!");
    this->Lhs = lhs;
    assert(rhs && "Rhs can not be null!");
    this->Rhs = rhs;
}
/// ----------------------------------------------------------

/// ----------------------------------------------------------
/// UnaryExprAST define code
UnaryExprAST::UnaryExprAST(const LineNo &lineNo, ASTBase *parent, Operator op, ExprAST *expr) : ExprAST(lineNo, parent) {
    this->Op = op;
    this->Expr =  expr;
}
/// ----------------------------------------------------------

/// ----------------------------------------------------------
/// LiteralExprAST define code
LiteralExprAST::LiteralExprAST(const LineNo &lineNo, ASTBase *parent) : ExprAST(lineNo, parent) {}

LiteralExprAST::LiteralExprAST(const LineNo &lineNo, ASTBase *parent, const std::string& str) : ExprAST(lineNo, parent) {
    this->Str = str;
}
/// ----------------------------------------------------------

/// ----------------------------------------------------------
/// NumberExprAST define code
NumberExprAST::NumberExprAST(const LineNo &lineNo, ASTBase *parent, char v) : ExprAST(lineNo, parent) {
    this->CValue = v;
    this->IsChar = true;
}

NumberExprAST::NumberExprAST(const LineNo &lineNo, ASTBase *parent, long long v) : ExprAST(lineNo, parent) {
    this->LValue = v;
    this->IsLong = true;
}


NumberExprAST::NumberExprAST(const LineNo &lineNo, ASTBase *parent, double v) : ExprAST(lineNo, parent) {
    this->DValue = v;
    this->IsDouble = true;
}

NumberExprAST::NumberExprAST(const LineNo &lineNo, ASTBase *parent, bool v) : ExprAST(lineNo, parent) {
    this->BValue = v;
    this->IsBool = true;
}
/// ----------------------------------------------------------

/// ----------------------------------------------------------
/// IdRefAST define code
IdRefAST::IdRefAST(const LineNo &lineNo, ASTBase *parent, const std::string& name) : ExprAST(lineNo, parent), IdName(name) {}
/// ----------------------------------------------------------

/// ----------------------------------------------------------
/// IdIndexedRefAST define code
IdIndexedRefAST::IdIndexedRefAST(const LineNo &lineNo, ASTBase *parent, const std::string& name) : ExprAST(lineNo, parent), IdName(name) {}

/// ----------------------------------------------------------

/// ----------------------------------------------------------
/// CallExprAST define code
CallExprAST::CallExprAST(const LineNo &lineNo, ASTBase *parent, const std::string& name) : ExprAST(lineNo, parent), FuncName(name){
    TheCallFunction = nullptr;
    IsCallStd = false;
}

#ifndef __USE_C_MODULE_TRANSLATION_METHOD__
llvm::Function *CallExprAST::getLLVMFunction() {
    if(TheCallFunction) {
        return TheCallFunction->getLLVMFunction();
    }
    return nullptr;
}
#endif
/// ----------------------------------------------------------

}
