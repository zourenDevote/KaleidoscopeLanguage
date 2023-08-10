#include "ast.h"
#include <cassert>

/// ----------------------------------------------------------
/// ASTBase code
ASTBase::ASTBase(const LineNo& lineNo) : LineMsg(lineNo), 
                                         Parent(nullptr), 
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
ProgramAST::ProgramAST(const LineNo& lineNo) : ASTBase(lineNo) {
}
/// ----------------------------------------------------------

/// ----------------------------------------------------------
/// ParamAST define code
ParamAST::ParamAST(const LineNo& lineNo, const std::string& name, KType type) : ASTBase(lineNo), Name(name) {
    this->Type = type;
    this->ArrayDim  = 0;
}
/// ----------------------------------------------------------

/// ----------------------------------------------------------
/// FuncAST define code
FuncAST::FuncAST(const LineNo& lineNo, const std::string& funcName, KType retType) : ASTBase(lineNo) {
    this->FuncName = funcName;
    this->RetType = retType;
    this->BlockStmt = nullptr;
}
/// ----------------------------------------------------------

/// ----------------------------------------------------------
/// InitializedAST define code
InitializedAST::InitializedAST(const LineNo& lineNo) : ExprAST(lineNo) {}
/// ----------------------------------------------------------

/// ----------------------------------------------------------
/// StructDefAST define code
StructDefAST::StructDefAST(const LineNo& lineNo) : ASTBase(lineNo) {}
/// ----------------------------------------------------------

/// ----------------------------------------------------------
/// StatementAST define code
StatementAST::StatementAST(const LineNo& line) : ASTBase(line) {}
/// ----------------------------------------------------------

/// ----------------------------------------------------------
/// ExprStmtAST define code
ExprStmtAST::ExprStmtAST(const LineNo& lineNo, ExprAST *expr) : StatementAST(lineNo) {
    Expr = expr;
}
/// ----------------------------------------------------------

/// ----------------------------------------------------------
/// VarDefAST define code
VarDefAST::VarDefAST(const LineNo& lineNo, const std::string& name, KType type) : StatementAST(lineNo) {
    this->VarName = name;
    this->VarType = type;
}   
/// ----------------------------------------------------------

/// ----------------------------------------------------------
/// BlockStmtAST define code
BlockStmtAST::BlockStmtAST(const LineNo& lineNo) : StatementAST(lineNo){
}
/// ----------------------------------------------------------

/// ----------------------------------------------------------
/// ReturnStmtAST define code
ReturnStmtAST::ReturnStmtAST(const LineNo& lineNo) : StatementAST(lineNo){
    RetExpr = nullptr;
}
/// ----------------------------------------------------------

/// ----------------------------------------------------------
/// BreakStmtAST define code
BreakStmtAST::BreakStmtAST(const LineNo& lineNo) : StatementAST(lineNo){
}
/// ----------------------------------------------------------

/// ----------------------------------------------------------
/// ContinueStmtAST define code
ContinueStmtAST::ContinueStmtAST(const LineNo& lineNo) : StatementAST(lineNo){
}
/// ----------------------------------------------------------

/// ----------------------------------------------------------
/// ForStmtAST define code
ForStmtAST::ForStmtAST(const LineNo& lineNo, ExprAST *expr1, ExprAST *expr2, ExprAST *expr3) : StatementAST(lineNo) {
    this->Expr1 = expr1;
    this->Expr2 = expr2;
    this->Expr3 = expr3;
    this->Stmt = nullptr;
}

ForStmtAST::ForStmtAST(const LineNo& lineNo) : StatementAST(lineNo) {
    this->Expr1 = nullptr;
    this->Expr2 = nullptr;
    this->Expr3 = nullptr;
    this->Stmt = nullptr;
}
/// ----------------------------------------------------------

/// ----------------------------------------------------------
/// WhileStmtAST define code
WhileStmtAST::WhileStmtAST(const LineNo& lineNo, ExprAST *cond) : StatementAST(lineNo) {
    this->Cond = cond;
    this->Stmt = nullptr;
}
/// ----------------------------------------------------------

/// ----------------------------------------------------------
/// IfStmtAST define code
IfStmtAST::IfStmtAST(const LineNo& lineNo, ExprAST *cond) : StatementAST(lineNo) {
    this->Cond = cond;
    this->Stmt = nullptr;
    this->Else = nullptr;
}
/// ----------------------------------------------------------

/// ----------------------------------------------------------
/// ExprAST define code
ExprAST::ExprAST(const LineNo& line) : ASTBase(line) {}
/// ----------------------------------------------------------

/// ----------------------------------------------------------
/// BinaryExprAST define code
BinaryExprAST::BinaryExprAST(const LineNo& lineNo, Operator op, ExprAST *lhs, ExprAST *rhs) : ExprAST(lineNo) {
    this->Op = op;
    assert(lhs && "Lhs can not be null!");
    this->Lhs = lhs;
    assert(rhs && "Rhs can not be null!");
    this->Rhs = rhs;
}
/// ----------------------------------------------------------

/// ----------------------------------------------------------
/// UnaryExprAST define code
UnaryExprAST::UnaryExprAST(const LineNo& lineNo, Operator op, ExprAST *expr) : ExprAST(lineNo) {
    this->Op = op;
    assert(expr && "Expr in unary expr can not be null!");
    this->Expr =  expr;
}
/// ----------------------------------------------------------

/// ----------------------------------------------------------
/// LiteralExprAST define code
LiteralExprAST::LiteralExprAST(const LineNo& lineNo) : ExprAST(lineNo) {}

LiteralExprAST::LiteralExprAST(const LineNo& lineNo, const std::string& str) : ExprAST(lineNo) {
    this->Str = str;
}
/// ----------------------------------------------------------

/// ----------------------------------------------------------
/// NumberExprAST define code
NumberExprAST::NumberExprAST(const LineNo& lineNo, char v) : ExprAST(lineNo) {
    this->CValue = v;
    this->IsChar = true;
}

NumberExprAST::NumberExprAST(const LineNo& lineNo, long v) : ExprAST(lineNo) {
    this->LValue = v;
    this->IsLong = true;
}


NumberExprAST::NumberExprAST(const LineNo& lineNo, double v) : ExprAST(lineNo) {
    this->DValue = v;
    this->IsDouble = true;
}

NumberExprAST::NumberExprAST(const LineNo& lineNo, bool v) : ExprAST(lineNo) {
    this->BValue = v;
    this->IsBool = true;
}
/// ----------------------------------------------------------

/// ----------------------------------------------------------
/// IdRefAST define code
IdRefAST::IdRefAST(const LineNo& lineNo, const std::string& name) : ExprAST(lineNo), IdName(name) {}

Value *IdRefAST::getLLVMValue() {
    if(Param) {
        return Param->getLLVMValue();
    }
    else if(VarDef) {
        return VarDef->getLLVMValue();
    }
    return nullptr;
}
/// ----------------------------------------------------------

/// ----------------------------------------------------------
/// IdIndexedRefAST define code
IdIndexedRefAST::IdIndexedRefAST(const LineNo& lineNo, const std::string& name) : ExprAST(lineNo), IdName(name) {}

Value *IdIndexedRefAST::getLLVMValue() {
    if(Param) {
        return Param->getLLVMValue();
    }
    else if(VarDef) {
        return VarDef->getLLVMValue();
    }
    return nullptr;
}
/// ----------------------------------------------------------

/// ----------------------------------------------------------
/// CallExprAST define code
CallExprAST::CallExprAST(const LineNo& lineNo, const std::string& name) : ExprAST(lineNo), FuncName(name){
    TheCallFunction = nullptr;
}

llvm::Function *CallExprAST::getLLVMFunction() {
    if(TheCallFunction) {
        return TheCallFunction->getLLVMFunction();
    }
    return nullptr;
}
/// ----------------------------------------------------------

