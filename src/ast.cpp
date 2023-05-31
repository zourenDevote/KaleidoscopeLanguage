#include "ast.h"
#include <cassert>

/// ----------------------------------------------------------
/// ASTBase code
ASTBase::ASTBase(const LineNo& lineNo) : LineMsg(lineNo), 
                                         Parent(nullptr), 
                                         Program(nullptr){}
                                        
void ASTBase::accept(AstVisitor *v) {
    v->visit(this);
}

void ASTBase::addChild(ASTBase *child) {
    this->Childs.push_back(child);
}

void ASTBase::setParent(ASTBase *parent) {
    this->Parent = parent;
}

void ASTBase::setLineNo(ASTBase *lineNo) {
    this->LineMsg = lineNo;
}

void ASTBase::setProgram(ProgramAST *prog) {
    this->Program = prog;
}

bool ASTBase::childEmpty() {
    return this->Childs.empty();
}

const ASTBase::NodeIter& ASTBase::cbegin() {
    return this->Childs->cbegin();
}

const ASTBase::NodeIter& ASTBase::cend() {
    return this->Childs->cend();
}

const ASTBase::NodeIter& ASTBase::crbegin() {
    return this->Childs->crbegin();
}

const ASTBase::NodeIter& ASTBase::crend() {
    return this->Childs->crend();
}

ASTBase::NodeIter ASTBase::begin() {
    return this->Childs->begin();
}

ASTBase::NodeIter ASTBase::end() {
    return this->Childs->end();
}

ASTBase::NodeIter ASTBase::rbegin() {
    return this->Childs->rbegin();
}

ASTBase::NodeIter ASTBase::rend() {
    return this->Childs->rend();
}

ASTBase *ASTBase::getChild(size_t index) {
    if(Childs.size() <= index) {
        return nullptr;
    }
    return Childs[index];
}

/// ----------------------------------------------------------

/// ----------------------------------------------------------
/// ProgramAST define code
ProgramAST::ProgramAST(const LineNo& lineNo) : ASTBase(lineNo) {
    setId(ProgramID);
}
/// ----------------------------------------------------------

/// ----------------------------------------------------------
/// ParamAST define code
ParamAST::ParamAST(const LineNo& lineNo, const std::string& name, KType type) : ASTBase(lineNo) {
    setId(FuncParamId);
    this->Name = name;
    this->Type = type;
    this->ArrayDim  = 0;
}
/// ----------------------------------------------------------

/// ----------------------------------------------------------
/// FuncAST define code
FuncAST::FuncAST(const LineNo& lineNo, const std::string& funcName, KType retType) : ASTBase(lineNo) {
    setId(FuncId);
    this->FuncName = funcName;
    this->RetType = retType;
}
/// ----------------------------------------------------------

/// ----------------------------------------------------------
/// InitializedAST define code
InitializedAST::InitializedAST(const LineNo& lineNo, ExprAST *expr) : ASTBase(lineNo) {
    setId(InitializeId);
    this->InitExpr = expr;
}
/// ----------------------------------------------------------

/// ----------------------------------------------------------
/// StructDefAST define code
StructDefAST::StructDefAST(const LineNo& lineNo) : ASTBase(lineNo) {
    setId(StructId);
}
/// ----------------------------------------------------------

/// ----------------------------------------------------------
/// VarDefAST define code
VarDefAST::VarDefAST(const LineNo& lineNo, const std::string& name, KType type) : ASTBase(lineNo) {
    setId(VarDefId);
    this->VarName = name;
    this->VarType = type;
}   
/// ----------------------------------------------------------

/// ----------------------------------------------------------
/// ForStmtAST define code
ForStmtAST::ForStmtAST(const LineNo& lineNo, ASTBase *expr1, ASTBase *expr2, ASTBase *expr3) : ASTBase(lineNo) {
    setId(ForStmtId);
    this->Expr1 = expr1;
    this->Expr2 = expr2;
    this->Expr3 = expr3;
}

ForStmtAST::ForStmtAST(const LineNo& lineNo) : ASTBase(lineNo) {
   setId(ForStmtId); 
    this->Expr1 = nullptr;
    this->Expr2 = nullptr;
    this->Expr3 = nullptr;
}
/// ----------------------------------------------------------

/// ----------------------------------------------------------
/// WhileStmtAST define code
WhileStmtAST::WhileStmtAST(const LineNo& lineNo, ASTBase *cond) : ASTBase(lineNo) {
    setId(WhileStmtId);
    this->Cond = cond;
}
/// ----------------------------------------------------------

/// ----------------------------------------------------------
/// IfStmtAST define code
IfStmtAST::IfStmtAST(const LineNo& lineNo, ASTBase *cond) : ASTBase(lineNo) {
    setId(IfStmtId);
    this->Cond = cond;
    this->Else = nullptr;
}
/// ----------------------------------------------------------

/// ----------------------------------------------------------
/// BinaryExprAST define code
BinaryExprAST::BinaryExprAST(const LineNo& lineNo, Operator op, ASTBase *lhs, ASTBase *rhs) : ASTBase(lineNo) {
    setId(BinExprId);
    this->Op = op;
    assert(lhs && "Lhs can not be null!");
    addChild(lhs);
    assert(rhs && "Rhs can not be null!");
    addChild(rhs);
}
/// ----------------------------------------------------------

/// ----------------------------------------------------------
/// UnaryExprAST define code
UnaryExprAST::UnaryExprAST(const LineNo& lineNo, Operator op, ASTBase *expr) : ASTBase(lineNo) {
    setId(UnaryExprId);
    this->Op = op;
    assert(expr && "Expr in unary expr can not be null!");
    addChild(expr);
}
/// ----------------------------------------------------------

/// ----------------------------------------------------------
/// LiteralExprAST define code
LiteralExprAST::LiteralExprAST(const LineNo& lineNo) : ASTBase(lineNo) {
    setId(LiteralId);
}

LiteralExprAST::LiteralExprAST(const LineNo& lineNo, const std::string& str) : ASTBase(lineNo) {
    setId(LiteralId);
    this->Str = str;
}
/// ----------------------------------------------------------

/// ----------------------------------------------------------
/// NumberExprAST define code
NumberExprAST::NumberExprAST(const LineNo& lineNo, char v) : ASTBase(lineNo) {
    setId(NumberId);
    this->LitValue.C = v;
    this->Type = Char;
}

NumberExprAST::NumberExprAST(const LineNo& lineNo, unsigned char v) : ASTBase(lineNo) {
    setId(NumberId);
    this->LitValue.Uc = v;
    this->Type = UChar;
}

NumberExprAST::NumberExprAST(const LineNo& lineNo, short v) : ASTBase(lineNo) {
    setId(NumberId);
    this->LitValue.S = v;
    this->Type = Short;
}

NumberExprAST::NumberExprAST(const LineNo& lineNo, unsigned short v) : ASTBase(lineNo) {
    setId(NumberId);
    this->LitValue.Us = v;
    this->Type = UShort;
}

NumberExprAST::NumberExprAST(const LineNo& lineNo, int v) : ASTBase(lineNo) {
    setId(NumberId);
    this->LitValue.I = v;
    this->Type = Int;
}

NumberExprAST::NumberExprAST(const LineNo& lineNo, unsigned int v) : ASTBase(lineNo) {
    setId(NumberId);
    this->LitValue.Ui = v;
    this->Type = UInt;
}

NumberExprAST::NumberExprAST(const LineNo& lineNo, long v) : ASTBase(lineNo) {
    setId(NumberId);
    this->LitValue.L = v;
    this->Type = Long;
}

NumberExprAST::NumberExprAST(const LineNo& lineNo, unsigned long v) : ASTBase(lineNo) {
    setId(NumberId);
    this->LitValue.Ul = v;
    this->Type = ULong;
}

NumberExprAST::NumberExprAST(const LineNo& lineNo, double v) : ASTBase(lineNo) {
    setId(NumberId);
    this->LitValue.D = v;
    this->Type = Double;
}

NumberExprAST::NumberExprAST(const LineNo& lineNo, float v) : ASTBase(lineNo) {
    setId(NumberId);
    this->LitValue.F = v;
    this->Type = Float;
}

NumberExprAST::NumberExprAST(const LineNo& lineNo, bool v) : ASTBase(lineNo) {
    setId(NumberId);
    this->LitValue.B = v;
    this->Type = Bool;
}
/// ----------------------------------------------------------

/// ----------------------------------------------------------
/// IdRefAST define code
IdRefAST::IdRefAST(const LineNo& lineNo, const std::string& name) : ASTBase(lineNo) {
    setId(IdRefId);
    this->IdName = name;
}

Value *IdRefAST::getLLVMValue() {
    if(Param) {
        return Param->getValue();
    }
    else if(VarDef) {
        return VarDef->getValue();
    }
    return nullptr;
}
/// ----------------------------------------------------------

/// ----------------------------------------------------------
/// IdIndexedRefAST define code
IdIndexedRefAST::IdIndexedRefAST(const LineNo& lineNo, const std::string& name) : ASTBase(lineNo) {
    setId(IdRefId);
    this->IdName = name;
}

Value *IdIndexedRefAST::getLLVMValue() {
    if(Param) {
        return Param->getValue();
    }
    else if(VarDef) {
        return VarDef->getValue();
    }
    return nullptr;
}
/// ----------------------------------------------------------

/// ----------------------------------------------------------
/// CallExprAST define code
CallExprAST::CallExprAST(const LineNo& lineNo, const std::string& name) : ASTBase(lineNo) {
    setId(CallId);
    this->FuncName = name;
}

llvm::Function *CallExprAST::getLLVMFunction() {
    if(TheCallFunction) {
        return TheCallFunction->getLLVMFunction();
    }
    return nullptr;
}
/// ----------------------------------------------------------

