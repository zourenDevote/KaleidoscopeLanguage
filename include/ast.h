

#ifndef KALE_AST_H
#define KALE_AST_H

// clang-format off

#include "common.h"

#ifndef __USE_C_MODULE_TRANSLATION_METHOD__
#include "llvm/IR/Function.h"
#include "llvm/IR/Value.h"
#include "llvm/IR/Type.h"
#endif

#include <vector>
#include <string>

#ifndef __USE_C_MODULE_TRANSLATION_METHOD__
using namespace llvm;
#endif

namespace kale {

struct LineNo {
    unsigned FileIndex;
    unsigned Row;
    unsigned Col;
};

class ProgramAST;
class AstVisitor;
class BlockStmtAST;
class StatementAST;
class ExprAST;
class InitializedAST;
class VariableAST;


/// ------------------------------------------------------------------------
/// @brief ASTBase define the base ast node, each ast node must extends this
/// node, in ast base, we decalre the LineMsg to save the (file line:column) 
/// message of ast to sources file, and define theirtual function accept
/// to accept theisitor forisit each ast node.
/// ------------------------------------------------------------------------
class ASTBase {
protected:
    LineNo      LineMsg;
    ASTBase     *Parent{nullptr};
    ProgramAST  *Program;

public:
    virtual void accept(AstVisitor &v) = 0;

public:
    ASTBase() = default;
    ASTBase(const LineNo&, ASTBase *);

   void setParent(ASTBase *parent);
   void setLineNo(const LineNo&);
   void setProgram(ProgramAST *prog);

   virtual ASTBase* deepCopy() { return nullptr; }

   LineNo      *getLineNo()    { return &LineMsg; }
   ASTBase     *getParent()    { return Parent; }
   ProgramAST  *getProgram()   { return Program; }

public:
    virtual KAstId getClassId() = 0;
    virtual const char* getAstName(){
        return "";
    }
};


/// ------------------------------------------------------------------------
/// @brief program ast express the whole source file, and it has it depends
/// and compile statue.
/// ------------------------------------------------------------------------
class ProgramAST : public ASTBase {
public:
    /// @brief this enum type express program compiled statue
    enum CompiledFlag {
        Success,
        Failed,
        NotCompiled,
    };

private:
    std::vector<ProgramAST*> DependentProg;             // 依赖项
    CompiledFlag             CompFlag{NotCompiled};     // 处理状态
    std::vector<ASTBase*>    ProgramElems;              //
public:
    INSERT_ENUM(ProgramId)
    static bool canCastTo(KAstId id) { return (id == ProgramId); }

    INSERT_ENUM_NAME(ProgramId)
public:
    ProgramAST() = default;
    explicit ProgramAST(const LineNo&, ASTBase *parent = nullptr);

    /// @brief 添加program的依赖项
    /// @param prog 
    void addDependentProg(ProgramAST *prog) { this->DependentProg.push_back(prog); }
    /// @brief 获取program的依赖项
    /// @return 
    const std::vector<ProgramAST*>& getDependentProgs() { return this->DependentProg; }
    /// @brief 设置编译状态
    /// @param flag 
    void setCompiledFlag(CompiledFlag flag) { CompFlag = flag; }
    /// @brief 添加编译元素
    /// @param elem
    void addCompElem    (ASTBase *elem) { ProgramElems.push_back(elem); }

    /// @brief 获取编译状态
    /// @return 
    CompiledFlag getCompiledFlag() const { return CompFlag; }

    const std::vector<ASTBase*>& getCompElems() { return ProgramElems; }
public:
    INSERT_ACCEPT
};


/// ------------------------------------------------------------------------
/// @brief ParamAST express the function param declare in kaleidoscope
/// ------------------------------------------------------------------------
class ParamAST : public ASTBase {
private:
    VariableAST *Id;

public:
    INSERT_ENUM(FuncParamId)
    static bool canCastTo(KAstId id) { return (id == FuncParamId); }

public:
    explicit ParamAST(const LineNo&, ASTBase*, VariableAST *);

    void setId(VariableAST *id) { Id = id; }

    VariableAST *getId() { return Id; }

public:
    INSERT_ACCEPT
};

/// ------------------------------------------------------------------------
/// @brief: DataTypeAST ast express the data type of kaleidoscope language
/// ------------------------------------------------------------------------
class DataTypeAST : public ASTBase {
public:
    DataTypeAST() = default;
    DataTypeAST(const LineNo&, ASTBase *, KType);

public:
    INSERT_ENUM(DataTypeId)
    static bool canCastTo(KAstId id) { return (id == DataTypeId); }

public:
    void setDataType(KType type) { DataType = type; }
    KType getDataType() { return DataType; }

private:
    KType DataType;

public:
    INSERT_ACCEPT;
};


/// ------------------------------------------------------------------------
/// @brief FuncAST express function define and function delcare in kaleidoscope
/// ------------------------------------------------------------------------
class FuncAST : public ASTBase {
private:
    using ParamIter = std::vector<ParamAST *>::iterator;
#ifndef __USE_C_MODULE_TRANSLATION_METHOD__
    Function                *LLVMFunc;        //
#endif
    std::string              FuncName;        // 函数名
    std::vector<ParamAST *>  FuncParams;      // 函数参数列表
    DataTypeAST             *RetType;         // 返回值类型
    BlockStmtAST            *BlockStmt;

public:
    INSERT_ENUM(FuncId)
    static bool canCastTo(KAstId id) { return (id == FuncId); }

public:
    explicit FuncAST(const LineNo&, ASTBase*, const std::string&);

#ifndef __USE_C_MODULE_TRANSLATION_METHOD__
    void setLLVMFunction(Function *func)            { LLVMFunc = func; }
#endif
    void setFuncName    (const std::string& name)   { FuncName = name; }
    void setRetType     (DataTypeAST *type)         { RetType = type; }
    void addFuncParam   (ParamAST *param)           { FuncParams.push_back(param); }
    void setBlockStmt   (BlockStmtAST *stmt)        { BlockStmt = stmt; }

    BlockStmtAST                    *getBlockStmt   () { return BlockStmt; }

#ifndef __USE_C_MODULE_TRANSLATION_METHOD__
    Function                        *getLLVMFunction() { return LLVMFunc; }
#endif
    const std::string               &getFuncName    () { return FuncName; }
    const std::vector<ParamAST *>   &getParams      () { return FuncParams; }
    DataTypeAST                     *getRetType     () { return RetType; }

    /// @brief 参数列表的迭代器
    /// @return 
    ParamIter arg_begin () { return FuncParams.begin(); }
    ParamIter arg_end   () { return FuncParams.end(); }
    
    /// @brief 参数列表是否为空
    /// @return 
    bool arg_empty      () { return FuncParams.empty(); }

    /// @brief 是否是函数声明
    /// @return 
    bool isFuncDeclare  () { return BlockStmt == nullptr; }

public:
    INSERT_ACCEPT
}; 


/// ------------------------------------------------------------------------
/// @brief Struct AST express struct define in kaleidoscope
/// @todo not support in this time
/// ------------------------------------------------------------------------
class StructDefAST : public ASTBase{
public:
    explicit StructDefAST(const LineNo&, ASTBase*);

public:
    INSERT_ENUM(StructId)
    static bool canCastTo(KAstId id) { return (id == StructId); }

public:
    INSERT_ACCEPT
};

/// ------------------------------------------------------------------------
/// @brief Base Statement AST express base statements class in kaleidoscope
/// ------------------------------------------------------------------------
class StatementAST : public ASTBase {
public:
    StatementAST() = default;
    explicit StatementAST(const LineNo&, ASTBase*);

public:
    INSERT_ENUM(StmtId)
    static bool canCastTo(KAstId id) { return (id == StmtId); }
};

/// ------------------------------------------------------------------------
/// @brief ExprStmtAST AST express expr statements
/// ------------------------------------------------------------------------
class ExprStmtAST : public StatementAST {
private:
    ExprAST *Expr;
public:
    ExprStmtAST() = default;
    explicit ExprStmtAST(const LineNo&, ASTBase*, ExprAST *expr);

public:
    void setExpr(ExprAST *expr) { Expr = expr; }

    ExprAST *getExpr() { return Expr; }
public:
    INSERT_ENUM(ExprStmtId)
    static bool canCastTo(KAstId id) { return (id == ExprStmtId || StatementAST::canCastTo(id)); }

public:
    INSERT_ACCEPT
};


/// ------------------------------------------------------------------------
/// @brief: IdDefAST ast express the id define in kaleidoscope language, such
///         as variable define, type ref, struct define...
/// ------------------------------------------------------------------------
class IdDefAST : public ASTBase {
public:
    IdDefAST() = default;
    IdDefAST(const LineNo&, ASTBase *, const std::string&);

public:
    INSERT_ENUM(IdDefId)
    static bool canCastTo(KAstId id) { return (id == IdDefId); }

    virtual bool isVariable()   { return false; }
    virtual bool isTypeRef()    { return false; }
    virtual bool isStructDef()  { return false; }

    void setDataType(DataTypeAST *ty)         { DataType = ty; }
    void setName    (const std::string &name) { Name = name; }

    DataTypeAST *getDataType()  { return DataType; }
    const char  *getName    ()  { return Name.c_str(); }

#ifndef __USE_C_MODULE_TRANSLATION_METHOD__
    void setLLVMValue(llvm::Value *v) { Value = v; }
    void setLLVMType(llvm::Type *ty) { VarLLVMType = ty; }
    llvm::Value *getLLVMValue() { return Value; }
    llvm::Type  *getVarLLVMType() { return VarLLVMType; }
#endif
private:
    DataTypeAST *DataType;
    std::string Name;

#ifndef __USE_C_MODULE_TRANSLATION_METHOD__
    /// llvm
    llvm::Value *Value = nullptr;
    llvm::Type* VarLLVMType = nullptr;
#endif
};


/// ------------------------------------------------------------------------
/// @brief Variable ast expression the variable that user defined
/// ------------------------------------------------------------------------
class VariableAST : public IdDefAST {
public:
    VariableAST() = default;
    VariableAST(const LineNo&, ASTBase *, const std::string&);

public:
    INSERT_ENUM(VariableId)
    static bool canCastTo(KAstId id) { return (id == VariableId || IdDefAST::canCastTo(id)); }


    void setIsStatic    ()      { VarFlag = (VarFlag & 0xFFFFFFFE) | 0x1; }
    void setIsConst     ()      { VarFlag = (VarFlag & 0xFFFFFFFD) | 0x2; }
    void setIsProtected ()      { VarFlag = (VarFlag & 0xFFFFFFE3) | 0x4; }
    void setIsPrivate   ()      { VarFlag = (VarFlag & 0xFFFFFFE3) | 0x8; }
    void setIsPublic    ()      { VarFlag = (VarFlag & 0xFFFFFFE3) | 0x10; }
    void setIsExtern    ()      { VarFlag = (VarFlag & 0xFFFFFFDF) | 0x20; }


    bool isStatic       ()      { return VarFlag & 0x1; }
    bool isConst        ()      { return VarFlag & 0x2; }
    bool isProtected    ()      { return VarFlag & 0x4; }
    bool isPrivate      ()      { return VarFlag & 0x8; }
    bool isPublic       ()      { return VarFlag & 0x10; }
    bool isExtern       ()      { return VarFlag & 0x20; }
    bool isArrray       ()      { return !Dims.empty(); }
    bool hasInitExpr    ()      { return InitExpr != nullptr; }

    unsigned                      getArrayDimSize    ()  { return Dims.size(); }
    const std::vector<ExprAST *> &getDims            ()  { return Dims; }
    ExprAST                      *getInitExpr        ()  { return InitExpr; }
    ExprAST                      *getIndexDimLength  (unsigned index) { return Dims[index]; }

    void addDims    (ExprAST *dim)            { Dims.push_back(dim); }
    void setInitExpr(ExprAST *expr)           { InitExpr = expr; }
private:
    unsigned VarFlag;                       // the flag that record the variable message
    std::vector<ExprAST *> Dims;            // the dimensions record of the variable
    ExprAST *InitExpr = nullptr;            // the init expr of the variable
public:
    INSERT_ACCEPT
};

/// ------------------------------------------------------------------------
/// @brief Var def ast expressar define andar extern in kaleidoscope.
/// ------------------------------------------------------------------------
class DataDeclAST : public StatementAST {
private:
    std::vector<VariableAST *> VarDecls;
public:
    INSERT_ENUM(DataDeclId)
    static bool canCastTo(KAstId id) { return (id == DataDeclId || StatementAST::canCastTo(id)); }

public:
    DataDeclAST(const LineNo&, ASTBase*);

public:
    void addVarDecl(VariableAST *v) { VarDecls.push_back(v); }

    const std::vector<VariableAST *>& getVarDecls() { return VarDecls; }

public:
    INSERT_ACCEPT
};

/// ------------------------------------------------------------------------
/// @brief BlockStmt AST express for statements in kaleidoscope
/// ------------------------------------------------------------------------
class BlockStmtAST : public StatementAST {
private:
    std::vector<StatementAST*> Stmts;
public:
    explicit BlockStmtAST(const LineNo&, ASTBase*);

public:
    INSERT_ENUM(BlockStmtId)
    static bool canCastTo(KAstId id) { return (id == BlockStmtId || StatementAST::canCastTo(id)); }

public:
    void addStmt(StatementAST *stmt) { Stmts.push_back(stmt); }

    const std::vector<StatementAST*>& getStmts() { return Stmts; }

public:
    INSERT_ACCEPT
};

/// ------------------------------------------------------------------------
/// @brief ReturnStmt AST express for return in kaleidoscope
/// ------------------------------------------------------------------------
class ReturnStmtAST : public StatementAST {
private:
    ExprAST *RetExpr = {nullptr};

public:
    explicit ReturnStmtAST(const LineNo&, ASTBase*);

public:
    INSERT_ENUM(ReturnStmtId)
    static bool canCastTo(KAstId id) { return (id == ReturnStmtId || StatementAST::canCastTo(id)); }

public:
    void    setRetExpr(ExprAST *expr) { RetExpr = expr; }

    ExprAST *getRetExpr() { return RetExpr; }

public:
    INSERT_ACCEPT
};

/// ------------------------------------------------------------------------
/// @brief BreakStmt AST express for break in kaleidoscope
/// ------------------------------------------------------------------------
class BreakStmtAST : public StatementAST {
public:
    BreakStmtAST() = default;
    BreakStmtAST(const LineNo&, ASTBase *);
public:
    INSERT_ENUM(BreakStmtId)
    static bool canCastTo(KAstId id) { return (id == BreakStmtId || StatementAST::canCastTo(id)); }

public:
    INSERT_ACCEPT
};

/// ------------------------------------------------------------------------
/// @brief ContinueStmt AST express for continue in kaleidoscope
/// ------------------------------------------------------------------------
class ContinueStmtAST : public StatementAST {
public:
    ContinueStmtAST(const LineNo&, ASTBase*);

public:
    INSERT_ENUM(ContinueStmtId)
    static bool canCastTo(KAstId id) { return (id == ContinueStmtId || StatementAST::canCastTo(id)); }

public:
    INSERT_ACCEPT
};


/// ------------------------------------------------------------------------
/// @brief ForStmt AST express for statement in kaleidoscope
/// ------------------------------------------------------------------------
class ForStmtAST : public StatementAST {
private:
    ExprAST      *Expr1;
    ExprAST      *Expr2;
    ExprAST      *Expr3;
    StatementAST *Stmt;

public:
    INSERT_ENUM(ForStmtId)
    static bool canCastTo(KAstId id) { return (id == ForStmtId || StatementAST::canCastTo(id)); }

public:
    ForStmtAST() = default;
    ForStmtAST(const LineNo&, ASTBase*, ExprAST*, ExprAST*, ExprAST*);
    ForStmtAST(const LineNo&, ASTBase*);

public:
    void setExpr1       (ExprAST *expr)      { Expr1 = expr; }
    void setExpr2       (ExprAST *expr)      { Expr2 = expr; }
    void setExpr3       (ExprAST *expr)      { Expr3 = expr; }
    void setStatement   (StatementAST *stmt) { Stmt = stmt; }

    ExprAST *getExpr1()          const { return Expr1; }
    ExprAST *getExpr2()          const { return Expr2; }
    ExprAST *getExpr3()          const { return Expr3; }
    StatementAST *getStatement()       { return Stmt; }

public:
    INSERT_ACCEPT
};


/// ------------------------------------------------------------------------
/// @brief WhileStmt AST express while statement in kaleidoscope
/// ------------------------------------------------------------------------
class WhileStmtAST : public StatementAST {
private:
    ExprAST      *Cond;
    StatementAST *Stmt;

public:
    INSERT_ENUM(WhileStmtId)
    static bool canCastTo(KAstId id) { return (id == WhileStmtId || StatementAST::canCastTo(id)); }

public: 
    explicit WhileStmtAST(const LineNo&, ASTBase*, ExprAST*);

public:
    void setCond     (ExprAST *cond)      { Cond = cond; }
    void setStatement(StatementAST *stmt) { Stmt = stmt; }

    ExprAST       *getCond()        const { return Cond; }
    StatementAST  *getStatement()         { return Stmt; }

public:
    INSERT_ACCEPT
};


/// ------------------------------------------------------------------------
/// @brief IfStmt AST express if statement in kaleidoscope
/// ------------------------------------------------------------------------
class IfStmtAST : public StatementAST {
private:
    ExprAST *Cond;
    StatementAST *Stmt;
    StatementAST *Else;

public:
    INSERT_ENUM(IfStmtId)
    static bool canCastTo(KAstId id) { return (id == IfStmtId || StatementAST::canCastTo(id)); }

public:
    explicit IfStmtAST(const LineNo&, ASTBase*, ExprAST*);

    void setCond        (ExprAST *cond)          { Cond = cond; }
    void setElse        (StatementAST *elseStmt) { Else = elseStmt; }
    void setStatement   (StatementAST *stmt)     { Stmt = stmt; }

    ExprAST      *getCond()      const { return Cond; }
    StatementAST *getElse()      const { return Else; }
    StatementAST *getStatement() const { return Stmt; }

public:
    INSERT_ACCEPT
};


/// ------------------------------------------------------------------------
/// @brief ExprAST the base class express ast node
/// ------------------------------------------------------------------------
class ExprAST : public ASTBase {
public:
    ExprAST() = default;
    explicit ExprAST(const LineNo& line, ASTBase*);
public:
    INSERT_ENUM(ExprId)
    static bool canCastTo(KAstId id) { return (id == ExprId); }

public:
    void setExprType(KType ty) { ExprType = ty; }
    void setIsSigned(bool sign) { IsSigned = sign; }
    KType getExprType() { return ExprType; }
    bool  isSign() { return IsSigned; }
private:
    KType ExprType;
    bool  IsSigned;
};

/// ------------------------------------------------------------------------
/// @brief Initialized AST express the initialize expression of kaleiodscope
///ar define. for example in source ode 'int a = 10;' and 'int b[10] = {1, 2, 3}'
/// the initialize ast express 10 and {1,2,3}
/// ------------------------------------------------------------------------
class InitializedAST : public ExprAST {
public:
    explicit InitializedAST(const LineNo&, ASTBase*);

public:
    INSERT_ENUM(InitializeId)
    static bool canCastTo(KAstId id) { return (id == InitializeId || ExprAST::canCastTo(id)); }

    void setInitExpr(InitializedAST *next) { Next = next; }
    void setExpr    (ExprAST *expr)        { InitExpr = expr; }

    InitializedAST *getInitExpr() { return Next; }
    ExprAST        *getExpr()     { return InitExpr; }

private:
    ExprAST *InitExpr;
    InitializedAST *Next;

public:
    INSERT_ACCEPT
};


/// ------------------------------------------------------------------------
/// @brief BinaryExpr AST expr binary expression in kaleidoscope for example
/// BinaryExpr AST can express 1+3, a >= 8, a[1] = 10...
/// ------------------------------------------------------------------------
class BinaryExprAST : public ExprAST {
private:
    Operator  Op;            // T <== The operator of the expression
    ExprAST  *Lhs;
    ExprAST  *Rhs;
public:
    INSERT_ENUM(BinExprId)
    static bool canCastTo(KAstId id) { return (id == BinExprId || ExprAST::canCastTo(id)); }

public:
    BinaryExprAST(const LineNo&, ASTBase*, Operator, ExprAST*, ExprAST*);
    
     void setOperator(Operator op)      { Op = op; }
     void setLhs     (ExprAST *lhs)     { Lhs = lhs; }
     void setRhs     (ExprAST *rhs)     { Rhs = rhs; }


    Operator  getExprOp () const { return Op; }
    ExprAST  *getLhs    ()       { return Lhs; }
    ExprAST  *getRhs    ()       { return Rhs; }

public:
    INSERT_ACCEPT
};


/// ------------------------------------------------------------------------
/// @brief Unary Expr AST express the unary expression in kaleiodscope such
/// as '-a', '!b', '!(a>b)' ...
/// ------------------------------------------------------------------------
class UnaryExprAST : public ExprAST {
private:
    Operator  Op;
    ExprAST  *Expr;
public:
    INSERT_ENUM(UnaryExprId)
    static bool canCastTo(KAstId id) { return (id == UnaryExprId || ExprAST::canCastTo(id)); }

public:
    UnaryExprAST() = default;
    UnaryExprAST(const LineNo&, ASTBase*, Operator, ExprAST*);

    void setOperator    (Operator op)   { this->Op = op; }
    void setUnaryExpr   (ExprAST *expr) { this->Expr = expr; }

    Operator getExprOp   () const { return Op; }
    ExprAST *getUnaryExpr()       { return Expr; }

public:
    INSERT_ACCEPT
};


/// ------------------------------------------------------------------------
/// @brief Literal Expr AST express the str literalalue in kaleidoscope
/// such as "aaa", "bbb", "cds".
/// ------------------------------------------------------------------------
class LiteralExprAST : public ExprAST {
private:
    std::string Str;

public:
    INSERT_ENUM(LiteralId)
    static bool canCastTo(KAstId id) { return (id == LiteralId || ExprAST::canCastTo(id)); }

public:
    LiteralExprAST() = default;
    LiteralExprAST(const LineNo&, ASTBase*);
    LiteralExprAST(const LineNo&, ASTBase*, const std::string&);

public:
    void appendCharacter(char c) { Str.push_back(c); }
    std::string getStr() const { return Str; }

public:
    INSERT_ACCEPT
};


/// ------------------------------------------------------------------------
/// @brief Number Expr AST express the number literat in kaleidoscope, such 
/// as 10, 10.0, true, false...
/// ------------------------------------------------------------------------
class NumberExprAST : public ExprAST {
private:
    bool    IsBool  {false};
    bool    IsLong  {false};
    bool    IsDouble{false};
    bool    IsChar  {false};
    bool    IsSigned{false};

    char    CValue;
    bool    BValue;
    long long    LValue;
    double  DValue;

public:
    INSERT_ENUM(NumberId)
    static bool canCastTo(KAstId id) { return (id == NumberId || ExprAST::canCastTo(id)); }

public: 
    explicit NumberExprAST(const LineNo&, ASTBase*, char);
    explicit NumberExprAST(const LineNo&, ASTBase*, long long);
    explicit NumberExprAST(const LineNo&, ASTBase*, double);
    explicit NumberExprAST(const LineNo&, ASTBase*, bool);

    void setIsSigned(bool isSigned)    { IsSigned = isSigned; }

    bool            isBoolLiteral   () { return IsBool; }
    bool            isLong          () { return IsLong; }
    bool            isDouble        () { return IsDouble; }
    bool            isChar          () { return IsChar; }
    bool            isSigned        () { return IsSigned; }
    char            getCValue    () { return CValue; }
    long long          getIValue    () { return LValue; }
    unsigned long long   getUIValue   () { return (unsigned long long)LValue; }
    double          getFValue  () { return DValue; }
    bool            getBoolValue    () { return BValue; }

public:
    INSERT_ACCEPT
}; 


/// ------------------------------------------------------------------------
/// @brief Id Ref AST express the id ref of variable
/// ------------------------------------------------------------------------
class IdRefAST : public ExprAST {
private:
    const std::string     IdName;             // var name
    IdDefAST             *Id;                 // define id

public:
    explicit IdRefAST(const LineNo&, ASTBase *, const std::string&);

public:
    INSERT_ENUM(IdRefId)
    static bool canCastTo(KAstId id) { return (id == IdRefId || ExprAST::canCastTo(id)); }

public:
    void setId(IdDefAST *id) { Id = id; }

#ifndef __USE_C_MODULE_TRANSLATION_METHOD__
    Value       *getLLVMValue()       { Id->getLLVMValue(); }
#endif
    IdDefAST    *getId()              { return Id; }
    std::string  getIdName()    const { return IdName; }

public:
    INSERT_ACCEPT
};


/// ------------------------------------------------------------------------
/// @brief Id Indexes Ref express the id indexed ref of arrayariable
/// ------------------------------------------------------------------------
class IdIndexedRefAST : public ExprAST {
private:
    std::vector<ExprAST*>  Indexes;             // indexes list
    const std::string      IdName;              // var name
    IdDefAST              *Id;                  // define id
public:
    explicit IdIndexedRefAST(const LineNo&, ASTBase*, const std::string&);

public:
    INSERT_ENUM(IdIndexedRefId)
    static bool canCastTo(KAstId id) { return (id == IdIndexedRefId || ExprAST::canCastTo(id)); }

public:
    void addIndex(ExprAST *expr)     { Indexes.push_back(expr); }
    void setId(IdDefAST *id) { Id = id; }

#ifndef __USE_C_MODULE_TRANSLATION_METHOD__
    Value                        *getLLVMValue()      { Id->getLLVMValue(); }
#endif
    IdDefAST                     *getId()             { return Id; }
    const std::vector<ExprAST*>  &getIndexes()  const { return Indexes; }
    std::string                   getIdName()   const { return IdName; }
public:
    INSERT_ACCEPT
};


/// ------------------------------------------------------------------------
/// @brief Call Expr AST express function call expr
/// ------------------------------------------------------------------------
class CallExprAST : public ExprAST {
private:
    FuncAST               *TheCallFunction = nullptr;          // 对应的FuncAST的定义
    const std::string      FuncName;                           // 函数名
    std::vector<ExprAST *> Args;
    bool                   IsCallStd;                          // flag to call std function
public:
    explicit CallExprAST(const LineNo&, ASTBase*, const std::string&);

public:
    INSERT_ENUM(CallId)
    static bool canCastTo(KAstId id) { return (id == CallId || ExprAST::canCastTo(id)); }

    void setFunction    (FuncAST *func)     { this->TheCallFunction = func; }
    void addArg         (ExprAST *arg)      { Args.push_back(arg); }
    void setIsCallStd   (bool flag)         { IsCallStd = flag; }

    const std::vector<ExprAST*> &getArgs()          const    { return this->Args; }
    const std::string           &getName()          const    { return this->FuncName; }
    bool                         isArgEmpty()       const    { return this->Args.empty(); }
    bool                         isCallStd()        const    { return this->IsCallStd; }

#ifndef __USE_C_MODULE_TRANSLATION_METHOD__
    llvm::Function              *getLLVMFunction();
#endif
    FuncAST                     *getFuncDef()                { return TheCallFunction; }

public:
    INSERT_ACCEPT
};

}

// clang-format on

#endif

