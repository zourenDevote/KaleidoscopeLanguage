

#ifndef KALEIDSCOPE_AST
#define KALEIDSCOPE_AST

#include <vector>
#include "token.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Type.h"


struct LineNo {
    unsigned Row;
    unsigned Col;
};

#define FRIEND_CLASS_DECLARE friend class IRGenerator;

class ExprAST;
class IdRefAST;
class VarDeclAST;
class VarDefAST;
class FuncDefAST;
class VarExternAST;
class FuncExternAST;
class AssignStmtAST;
class BlockStmtAST;


class ProgramAST {
private:
    std::vector<VarDefAST*>     GlobalDefs;
    std::vector<FuncDefAST*>    FuncDefs;
    std::vector<VarExternAST*>  VarExterns;  
    std::vector<FuncExternAST*> FuncExterns;
public:
    FRIEND_CLASS_DECLARE

    ProgramAST() = default;
    /* 添加变量定义节点 */
    void addVarDef(VarDefAST *node) { GlobalDefs.push_back(node); }
    /* 添加函数定义节点 */
    void addFuncDef(FuncDefAST *node) { FuncDefs.push_back(node); }
    /* 添加变量声明节点 */
    void addVarExtern(VarExternAST *node) { VarExterns.push_back(node); }
    /* 添加函数声明节点 */
    void addFuncExtern(FuncExternAST *node) { FuncExterns.push_back(node); }
};

enum StmtId {
    VarDefId,
    ForStmtId,
    WhileStmtId,
    IfStmtId,
    AssignStmtId,
    ExprStmtId,
    BlockStmtId,
    ReturnStmtId
};

class StatementAST {
public:
    StatementAST() = default;

    virtual StmtId stmtId() = 0;
};

class VarDeclAST {
private:
    std::string VarName;
    std::vector<int> ArrayDim;
    Token Type;
    ExprAST *InitExpr{nullptr};
    llvm::Value *LLVMVal{nullptr};
public:
    FRIEND_CLASS_DECLARE

    VarDeclAST() = default;

    bool isArray() { return !ArrayDim.empty(); }
    bool hasInitExpr() { return InitExpr != nullptr; }

    void setVarName(const std::string& name) { VarName = name; }
    void addArrayLen(int dim) { ArrayDim.push_back(dim); }
    void setInitExpr(ExprAST *node) { InitExpr = node; }
    void setType(Token type) { Type = type; }

    void setLLVMValue(llvm::Value *val) { LLVMVal = val; }
};


class VarDefAST : public StatementAST {
private:
    std::vector<VarDeclAST*> VarDecls;
    Token VarType;
public:
    FRIEND_CLASS_DECLARE

    StmtId stmtId() override { return VarDefId; }

    VarDefAST(Token tk) : VarType(tk) {}

    bool isIntType() { return VarType == tok_int; }
    bool isDoubleType() { return VarType == tok_double; }
    bool isBoolType() { return VarType == tok_bool; }

    /* 添加变量定义 */
    void addVarDecl(VarDeclAST *node) { VarDecls.push_back(node); node->setType(VarType); } 
};

class VarExternAST {
private:
    std::vector<VarDeclAST *> VarDecls;
    Token VarType;
public:
    FRIEND_CLASS_DECLARE

    VarExternAST(Token tk) : VarType(tk) {}

    bool isIntType() { return VarType == tok_int; }
    bool isDoubleType() { return VarType == tok_double; }
    bool isBoolType() { return VarType == tok_bool; }

    /* 添加变量定义 */
    void addVarDecl(VarDeclAST *node) { VarDecls.push_back(node); node->setType(VarType); }     
};


class ParamAST {
private:
    std::string ParamName;
    std::vector<int> ArrayDim;
    int ArraySize{0};
    Token Type;
    llvm::Value *Val{nullptr};
public:
    FRIEND_CLASS_DECLARE

    ParamAST(const std::string name, Token ty) : ParamName(name), Type(ty) {}

    bool isIntType() { return Type == tok_int; }
    bool isDoubleType() { return Type == tok_double; }
    bool isBoolType() { return Type == tok_bool; }
    
    void setIsArray()  {ArraySize++;}
    void setParamName(const std::string& name) { ParamName = name; }
    void addArrayLen(int dim) { ArrayDim.push_back(dim); }
    void setLLVMValue(llvm::Value *v) { Val = v; }
};

class FuncDefAST {
private:
    std::string FuncName;
    Token RetTy;
    std::vector<ParamAST *> ParamLists;
    BlockStmtAST* Statements{nullptr};
    llvm::Function *LLVMFunction{nullptr};
public:
    FRIEND_CLASS_DECLARE

    FuncDefAST(const std::string& name, Token ret) : FuncName(name), RetTy(ret) {}

    void setFuncName(const std::string& name) { FuncName = name; }
    void setRetTy(Token ret) { RetTy = ret; }
    void addParam(ParamAST *node) { ParamLists.push_back(node); }
    void setStmt(BlockStmtAST *node) { Statements = node; }

    void setLLVMFunction(llvm::Function *func) { LLVMFunction = func; }
};

class FuncExternAST {
private:
    std::string FuncName;
    Token RetTy;
    std::vector<ParamAST *> ParamLists;
    llvm::Function *LLVMFunction{nullptr};
public:
    FRIEND_CLASS_DECLARE

    FuncExternAST(const std::string& name, Token ret) : FuncName(name), RetTy(ret) {}
    void setFuncName(const std::string& name) { FuncName = name; }
    void setRetTy(Token ret) { RetTy = ret; }
    void addParam(ParamAST *node) { ParamLists.push_back(node); } 

    void setLLVMFunction(llvm::Function *func) { LLVMFunction = func; }
};


class BlockStmtAST : public StatementAST {
private:
    std::vector<StatementAST*> Statements;
public:
    FRIEND_CLASS_DECLARE;

    StmtId stmtId() override { return BlockStmtId; }

    BlockStmtAST() = default;

    void addStatement(StatementAST *stmt) { Statements.push_back(stmt); }
};

class IfStmtAST : public StatementAST {
private:
    ExprAST *Cond;
    StatementAST *Statement;
    StatementAST *ElseStatement{nullptr};
public:
    FRIEND_CLASS_DECLARE

    StmtId stmtId() override { return IfStmtId; }

    IfStmtAST(ExprAST *cond, StatementAST *stmt) : Cond(cond), Statement(stmt) {}

    void setCond(ExprAST *cond) { Cond = cond; }
    void setStatement(StatementAST *stmt) { Statement = stmt; }
    void setElseStatement(StatementAST *stmt) { ElseStatement = stmt; }
};

class ForStmtAST : public StatementAST {
private:
    IdRefAST *Id;
    ExprAST *AssignExpr{nullptr};
    ExprAST *CondExpr{nullptr};
    ExprAST *StepExpr{nullptr};
    StatementAST *Statement{nullptr};
public:
    FRIEND_CLASS_DECLARE

    StmtId stmtId() override { return ForStmtId; }

    ForStmtAST() = default;

    void setId(IdRefAST *id) { Id = id; }
    void setAssignExpr(ExprAST *expr) { AssignExpr = expr; }
    void setCondExpr(ExprAST *expr) { CondExpr = expr; }
    void setStepExpr(ExprAST *expr) { StepExpr = expr; }
    void setStatement(StatementAST *stmt) { Statement = stmt; }
};

class WhileStmtAST : public StatementAST {
private:
    ExprAST *Cond;
    StatementAST *Statement{nullptr};
public:
    FRIEND_CLASS_DECLARE

    StmtId stmtId() override { return WhileStmtId; }

    WhileStmtAST(ExprAST *cond) : Cond(cond) { }

    void setCond(ExprAST *cond) { Cond = cond; }
    void setStatement(StatementAST *stmt) { Statement = stmt; }
};

class AssignStmtAST : public StatementAST {
private:
    IdRefAST *Id;
    ExprAST *Expr;
public:
    FRIEND_CLASS_DECLARE;

    StmtId stmtId() override { return AssignStmtId; }

    AssignStmtAST(IdRefAST *id, ExprAST *expr) : Id(id), Expr(expr), StatementAST() {}

    void setId(IdRefAST *id) { Id = id; }
    void setExpr(ExprAST *expr) { Expr = expr; }
};

class ExprStmtAST : public StatementAST {
private:
    ExprAST *Expr;
public:
    FRIEND_CLASS_DECLARE

    StmtId stmtId() override { return ExprStmtId; }

    ExprStmtAST(ExprAST *expr) : Expr(expr), StatementAST() {}

    void setExpr(ExprAST *expr) { Expr = expr; } 
};

class ReturnStmtAST : public StatementAST {
private:
    ExprAST *Expr{nullptr};
public:
    FRIEND_CLASS_DECLARE

    StmtId stmtId() override { return ReturnStmtId; }

    ReturnStmtAST(ExprAST *expr = nullptr) : Expr(expr) {}  

    void setExpr(ExprAST *expr) { Expr = expr; } 
};

enum Operator{
    Plus, // +
    Sub,  // -
    Mul,  // *
    Div,  // /
    Eq,   // == 
    Neq,  // !=
    Lt,   // <
    LtEq, // <=
    Gt,   // >
    GtEq, // >=
    Not   // !
};

enum ExprId {
    BinExprId,
    UnaryExprId,
    IdRefExprId,
    CallExprId,
    DoubleExprId,
    IntExprId,
    BoolExprId
};

class ExprAST {
public:
    FRIEND_CLASS_DECLARE

    virtual ExprId exprId() = 0;

    ExprAST() {}
};

class BinExprAST : public ExprAST {
private:
    ExprAST *Lhs;
    ExprAST *Rhs;
    Operator Op;
public:
    FRIEND_CLASS_DECLARE

    ExprId exprId() override { return BinExprId; }

    BinExprAST(Operator op, ExprAST *lhs, ExprAST *rhs) : Op(op), Lhs(lhs), Rhs(rhs), ExprAST() {}

    void setLhs(ExprAST *lhs) { Lhs = lhs; }
    void setRhs(ExprAST *rhs) { Rhs = rhs; }
    void setOp(Operator op) { Op = op; }
};

class UnaryExprAST : public ExprAST {
private:
    ExprAST *Expr;
    Operator Op;
public:
    FRIEND_CLASS_DECLARE

    ExprId exprId() override { return UnaryExprId; }

    UnaryExprAST(Operator op, ExprAST *expr) : Op(op), Expr(expr), ExprAST() {}

    void setExpr(ExprAST *expr) { Expr = expr; }
    void setOp(Operator op) { Op = op; }
};

class IdRefAST : public ExprAST{
private:
    VarDeclAST *Decl{nullptr};
    ParamAST *Param{nullptr};
    std::vector<ExprAST*> ArrayIndexes;
    std::string IdName; 
public:
    FRIEND_CLASS_DECLARE;

    ExprId exprId() override { return IdRefExprId; }

    IdRefAST(const std::string& name, VarDeclAST *decl) : IdName(name), Decl(decl), ExprAST() {}

    void addArrayIndex(ExprAST* index) { ArrayIndexes.push_back(index); }
    void setVarDecl(VarDeclAST *decl) { Decl = decl; }
    void setParam(ParamAST *param) { Param = param; }
    void setIdName(const std::string& name) { IdName = name; }
};

class CallExprAST : public ExprAST {
private:
    FuncDefAST *FuncDef{nullptr};
    FuncExternAST *FuncExtern{nullptr};
    std::vector<ExprAST *> Params;
    std::string FuncName;
public:
    FRIEND_CLASS_DECLARE

    ExprId exprId() override { return CallExprId; }

    CallExprAST(const std::string& name) : FuncName(name), ExprAST() {}

    void setFuncDef(FuncDefAST *def) { FuncDef = def; }
    void setFuncExtern(FuncExternAST *funcExtern) { FuncExtern = funcExtern; }
    void addParam(ExprAST *param) { Params.push_back(param); }
    void setFuncName(const std::string& name) { FuncName = name; }
};

class DoubleExprAST : public ExprAST {
private:
    double Val;

public:
    FRIEND_CLASS_DECLARE

    ExprId exprId() override { return DoubleExprId; }

    DoubleExprAST(double v) : Val(v), ExprAST() {} 
};

class IntExprAST : public ExprAST {
private:
    int Val;
public:
    FRIEND_CLASS_DECLARE

    ExprId exprId() override { return IntExprId; }

    IntExprAST(int v) : Val(v), ExprAST() {} 
};

class BoolExprAST : public ExprAST {
private:
    bool Val;

public:
    FRIEND_CLASS_DECLARE

    ExprId exprId() override { return BoolExprId; }

    BoolExprAST(bool v) : Val(v), ExprAST() {} 
};
 

#endif

