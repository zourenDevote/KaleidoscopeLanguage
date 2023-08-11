

#ifndef KALEIDSCOPE_AST
#define KALEIDSCOPE_AST

// clang-format off

#include "common.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Value.h"
#include "llvm/IR/Type.h"

#include <vector>

using namespace llvm;

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
    ASTBase(const LineNo&);

   void setParent(ASTBase *parent);
   void setLineNo(const LineNo&);
   void setProgram(ProgramAST *prog);

   virtual ASTBase* deepCopy() { return nullptr; }
    
   ASTBase     *getChild(unsigned int index);
   LineNo      *getLineNo()    { return &LineMsg; }
   ASTBase     *getParent()    { return Parent; }
   ProgramAST  *getProgram()   { return Program; }

public:
    virtual KAstId getClassId() = 0;
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

public:
    ProgramAST() = default;
    explicit ProgramAST(const LineNo&);

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
    const std::string Name;
    KType             Type;
    std::vector<int>  ArrayDims;
    int               ArrayDim;
    bool              IsConst;
    Value            *Val;

public:
    INSERT_ENUM(FuncParamId)
    static bool canCastTo(KAstId id) { return (id == FuncParamId); }

public:
    explicit ParamAST(const LineNo&, const std::string&, KType);

    void addArrayDim    (int dim)       { ArrayDims.push_back(dim); }
    void arrayDimAdd    ()              { ArrayDim++; }
    void setLLVMValue   (Value *v)      { Val = v; }
    void setIsConst     (bool isConst)  { IsConst = isConst; }

    const std::string       &getParamName() { return Name; }
    KType                    getParamType() { return Type; }
    Value                   *getLLVMValue() { return Val; }
    const std::vector<int>  &getArrayDims() { return ArrayDims; }
    int                      getArrayDim () { return ArrayDim; }
    bool                     isConst     () { return IsConst; }

public:
    INSERT_ACCEPT
};


/// ------------------------------------------------------------------------
/// @brief FuncAST express function define and function delcare in kaleidoscope
/// ------------------------------------------------------------------------
class FuncAST : public ASTBase {
private:
    using ParamIter = std::vector<ParamAST *>::iterator;
    Function                *LLVMFunc;        //
    std::string              FuncName;        // 函数名
    std::vector<ParamAST *>  FuncParams;      // 函数参数列表
    KType                    RetType;         // 返回值类型
    BlockStmtAST            *BlockStmt;

public:
    INSERT_ENUM(FuncId)
    static bool canCastTo(KAstId id) { return (id == FuncId); }

public:
    explicit FuncAST(const LineNo&, const std::string&, KType);

    void setLLVMFunction(Function *func)            { LLVMFunc = func; }
    void setFuncName    (const std::string& name)   { FuncName = name; }
    void setRetType     (KType type)                { RetType = type; }
    void addFuncParam   (ParamAST *param)           { FuncParams.push_back(param); }
    void setBlockStmt   (BlockStmtAST *stmt)        { BlockStmt = stmt; }

    BlockStmtAST                    *getBlockStmt   () { return BlockStmt; }
    Function                        *getLLVMFunction() { return LLVMFunc; }
    const std::string               &getFuncName    () { return FuncName; }
    const std::vector<ParamAST *>   &getParams      () { return FuncParams; }
    KType                            getRetType     () { return RetType; }

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
    explicit StructDefAST(const LineNo&);

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
    explicit StatementAST(const LineNo&);

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
    explicit ExprStmtAST(const LineNo&, ExprAST *expr);

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
/// @brief Var def ast expressar define andar extern in kaleidoscope.
/// ------------------------------------------------------------------------
class VarDefAST : public StatementAST {
private:
    StructDefAST    *StructDef{nullptr};    // 对应的结构体变量定义指针，暂时不支持，留空
    std::string      VarName;               // 变量名
    KType            VarType;               // 变量类型
    bool             IsConst;               // 是否是const
    Value           *Val      {nullptr};    // 对应的llvm value
    ExprAST         *Init     {nullptr};    // initialize expression

public:
    INSERT_ENUM(VarDefId)
    static bool canCastTo(KAstId id) { return (id == VarDefId || StatementAST::canCastTo(id)); }

public:
    VarDefAST(const LineNo&, const std::string&, KType);

public:
    void setStructDefAST    (StructDefAST *sdef)      { StructDef = sdef; }
    void setVarName         (const std::string& name) { VarName = name; }
    void setVarType         (KType type)              { VarType = type; }
    void setIsConst         (bool flag)               { IsConst = flag; }
    void setLLVMValue       (Value *v)                { Val = v; }
    void setInitExpr        (ExprAST *init)           { Init = init; }
    
    StructDefAST *getStructDefAST() { return StructDef; }
    std::string   getVarName     () { return VarName; }
    Value        *getLLVMValue   () { return Val; }
    ExprAST      *getInitExpr    () { return Init; }
    KType         getVarType     () { return VarType; }
    bool          isConst        () { return IsConst; }

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
    explicit BlockStmtAST(const LineNo&);

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
    explicit ReturnStmtAST(const LineNo&);

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
    BreakStmtAST(const LineNo&);

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
    ContinueStmtAST(const LineNo&);

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
    ForStmtAST(const LineNo&, ExprAST*, ExprAST*, ExprAST*);
    ForStmtAST(const LineNo&);

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
    explicit WhileStmtAST(const LineNo&, ExprAST*);

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
    explicit IfStmtAST(const LineNo&, ExprAST*);

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
    explicit ExprAST(const LineNo& line);
public:
    INSERT_ENUM(ExprId)
    static bool canCastTo(KAstId id) { return (id == ExprId); }
};

/// ------------------------------------------------------------------------
/// @brief Initialized AST express the initialize expression of kaleiodscope
///ar define. for example in source ode 'int a = 10;' and 'int b[10] = {1, 2, 3}'
/// the initialize ast express 10 and {1,2,3}
/// ------------------------------------------------------------------------
class InitializedAST : public ExprAST {
public:
    explicit InitializedAST(const LineNo&);

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
    BinaryExprAST(const LineNo&, Operator, ExprAST*, ExprAST*);
    
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
    UnaryExprAST(const LineNo&, Operator, ExprAST*);

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
    LiteralExprAST(const LineNo&);
    LiteralExprAST(const LineNo&, const std::string&);

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

    char    CValue;
    bool    BValue;
    long    LValue;
    double  DValue;

public:
    INSERT_ENUM(NumberId)
    static bool canCastTo(KAstId id) { return (id == NumberId || ExprAST::canCastTo(id)); }

public: 
    explicit NumberExprAST(const LineNo&, char);
    explicit NumberExprAST(const LineNo&, long);
    explicit NumberExprAST(const LineNo&, double);
    explicit NumberExprAST(const LineNo&, bool);

    bool            isBoolLiteral   () { return IsBool; }
    bool            isLong          () { return IsLong; }
    bool            isDouble        () { return IsDouble; }
    char            getCharValue    () { return CValue; }
    unsigned char   getUCharValue   () { return (unsigned char)LValue; }
    short           getShortValue   () { return (short)LValue; }
    unsigned short  getUShortValue  () { return (unsigned short)LValue; }
    int             getIntValue     () { return (int)LValue; }
    unsigned int    getUIntValue    () { return (unsigned int)LValue; }
    long            getLongValue    () { return LValue; }
    unsigned long   getULongValue   () { return (unsigned long)LValue; }
    double          getDoubleValue  () { return DValue; }
    float           getFloatValue   () { return (float)DValue; }
    bool            getBoolValue    () { return BValue; }\

public:
    INSERT_ACCEPT
}; 


/// ------------------------------------------------------------------------
/// @brief Id Ref AST express the id ref of variable
/// ------------------------------------------------------------------------
class IdRefAST : public ExprAST {
private:
    const std::string     IdName;             //ar name
    ParamAST             *Param{nullptr};     // param define
    VarDefAST            *VarDef{nullptr};    //ar define

public:
    explicit IdRefAST(const LineNo&, const std::string&);

public:
    INSERT_ENUM(IdRefId)
    static bool canCastTo(KAstId id) { return (id == IdRefId || ExprAST::canCastTo(id)); }

public:
    void setVarDef  (VarDefAST*ar)      { VarDef =ar; }
    void setParam   (ParamAST *param)   { Param = param; }

    Value       *getLLVMValue();
    ParamAST    *getParam()     const { return Param; }
    VarDefAST   *getVarDef()    const { return VarDef; }
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
    ParamAST              *Param{nullptr};      // param define
    VarDefAST             *VarDef{nullptr};     // var define
public:
    explicit IdIndexedRefAST(const LineNo&, const std::string&);

public:
    INSERT_ENUM(IdIndexedRefId)
    static bool canCastTo(KAstId id) { return (id == IdIndexedRefId || ExprAST::canCastTo(id)); }

public:
    void addIndex   (ExprAST *expr)     { Indexes.push_back(expr); }
    void setVarDef  (VarDefAST *var)    { VarDef = var; }
    void setParam   (ParamAST *param)   { Param = param; }

    Value                        *getLLVMValue();
    ParamAST                     *getParam()    const { return Param; }
    VarDefAST                    *getVarDef()   const { return VarDef; }
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
public:
    explicit CallExprAST(const LineNo&, const std::string&);

public:
    INSERT_ENUM(CallId)
    static bool canCastTo(KAstId id) { return (id == CallId || ExprAST::canCastTo(id)); }

    void setLLVMFunction(FuncAST *func)     { this->TheCallFunction = func; }
    void addArg         (ExprAST *arg)      { Args.push_back(arg); }

    const std::vector<ExprAST*> &getArgs()          const    { return this->Args; }
    const std::string           &getName()          const    { return this->FuncName; }
    bool                         isArgEmpty()       const    { return this->Args.empty(); }
    llvm::Function              *getLLVMFunction();

public:
    INSERT_ACCEPT
};

// clang-format on

#endif

