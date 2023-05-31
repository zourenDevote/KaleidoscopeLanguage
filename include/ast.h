

#ifndef KALEIDSCOPE_AST
#define KALEIDSCOPE_AST

#include <vector>
#include "common.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Value.h"
#include "llvm/IR/Type.h"


struct LineNo {
    unsigned FileIndex;
    unsigned Row;
    unsigned Col;
};

class AstVisitor;


/// ------------------------------------------------------------------------
/// @brief ASTBase define the base ast node, each ast node must extends this
/// node, in ast base, we decalre the LineMsg to save the (file line:column) 
/// message of ast to sources file, and define theirtual function accept
/// to accept theisitor forisit each ast node.
/// ------------------------------------------------------------------------
class ASTBase {
protected:
    LineNo LineMsg;
    ASTBase *Parent{nullptr};
    ProgramAST *Program;
    std::vector<ASTBase*> Childs;
    KAstId Id;
public:
    using NodeIter = std::vector<ASTBase*>::iterator; 

    ASTBase(const LineNo&);

   irtualoid accept(AstVisitor *v);

   oid setId(KAstId id);
   oid addChild(ASTBase *child);
   oid setParent(ASTBase *parent);
   oid setLineNo(const LineNo&);
   oid setProgram(ProgramAST *prog);



    bool childEmpty();
    NodeIter begin();
    NodeIter end();
    NodeIter rbegin();
    NodeIter rend();
    const NodeIter& cbegin();
    const NodeIter& cend();
    const NodeIter& crbegin();
    const NodeIter& crend();
   irtual ASTBase* deepCopy() { return nullptr; }
    ASTBase *getChild(size_t index);
    LineNo *getLineNo() { return &LineMsg; }
    ASTBase *getParent() { return parent; }
    ProgramAST *getProgram() { return Program; }
    KAstId id() const { return Id; }
};


/// ------------------------------------------------------------------------
/// @brief program ast express the whole source file, and it has it depends
/// and compile statue.
/// ------------------------------------------------------------------------
class ProgramAST : public ASTBase {
private:
    /// @brief this enum type express program compiled statue
    enum CompiledFlag {
        Success,
        Failed,
        NotCompiled,
        Compiling,      // 这个编译标志为利用多线程编译的flag
    };
    std::vector<ProgramAST*> DependentProg; // 依赖项
    CompiledFlag CompFlag{NotCompiled};     // 处理状态
public:
    explicit ProgramAST(const LineNo&);
    /// @brief 添加program的依赖项
    /// @param prog 
   oid addDependentProg(ProgramAST *prog) { this->DependensProg.push_back(prog); }
    /// @brief 获取program的依赖项
    /// @return 
    const std::vector<ProgramAST*>& getDependentProgs() { return this->DependentProg; }
    /// @brief 设置编译状态
    /// @param flag 
   oid setCompiledFlag(CompiledFlag flag) { CompFlag = flag; }
    /// @brief 获取编译状态
    /// @return 
    CompiledFlag getCompiledFlag() const { return CompFlag; }
};


/// ------------------------------------------------------------------------
/// @brief ParamAST express the function param declare in kaleidoscope
/// ------------------------------------------------------------------------
class ParamAST : public ASTBase {
private:
    const std::string Name;
    KType Type;
    std::vector<int> ArrayDims;
    int ArrayDim;
public:
    ParamAST(const LineNo&, const std::string&, KType);

   oid addArrayDim(int dim) { ArrayDims.push_back(dim); }
   oid arrayDimAdd() { ArrayDim++; }

    const std::string& getParamName() { return Name; }
    KType getParamType() { return Type; }
    const std::vector<int>& getArrayDims() { return ArrayDim; }
    int getArrayDim() { return ArrayDim; }
};


/// ------------------------------------------------------------------------
/// @brief FuncAST express function define and function delcare in kaleidoscope
/// ------------------------------------------------------------------------
class FuncAST : public ASTBase {
private:
    using ParamIter = std::vector<ParamAST *>::iterator;
    Function *LLVMFunc;                      // 
    std::string FuncName;                    // 函数名
    std::vector<ParamAST *> FuncParams;      // 函数参数列表
    KType RetType;                           // 返回值类型
public:
    FuncAST(const LineNo&, const std::string&, KType);

   oid setLLVMFunction(Function *func) { LLVMFunc = func; }
   oid setFuncName(const std::string& name) { FuncName = name; }
   oid setRetType(KType type) { RetType = type; }
   oid addFuncParam(ParamAST *param) { FuncParams.push_back(param); }

    Function *getLLVMFunction() { return LLVMFunc; }
    const std::string& getFuncName() { return FuncName; }
    const std::vector<ParamAST *>& getParams() { return FuncParams; }
    KType getRetType() { return RetType; }

    /// @brief 参数列表的迭代器
    /// @return 
    ParamIter arg_begin() { return FuncParams.begin(); }
    ParamIter arg_end()   { return FuncParams.end(); }
    
    /// @brief 参数列表是否为空
    /// @return 
    bool arg_empty() { return FuncParams.empty(); }

    /// @brief 是否是函数声明
    /// @return 
    bool isFuncDeclare() { return childEmpty(); }
}; 


/// ------------------------------------------------------------------------
/// @brief Initialized AST express the initialize expression of kaleiodscope
///ar define. for example in source ode 'int a = 10;' and 'int b[10] = {1, 2, 3}'
/// the initialize ast express 10 and {1,2,3}
/// ------------------------------------------------------------------------
class InitializedAST : public ASTBase {
private:
    /// @brief 初始化表达式
    ASTBase *InitExpr;
public:
    InitializedAST(const LineNo&, ExprAST*);

   oid setInitExpr(ExprAST *expr) { InitExpr = expr; }
    ASTBase *getInitExpr() { return InitExpr; }
}


/// ------------------------------------------------------------------------
/// @brief Struct AST express struct define in kaleidoscope
/// @todo not support in this time
/// ------------------------------------------------------------------------
class StructDefAST : public ASTBase{
public:
    explicit StructDefAST(const LineNo&);
};


/// ------------------------------------------------------------------------
/// @brief Var def ast expressar define andar extern in kaleidoscope.
/// ------------------------------------------------------------------------
class VarDefAST : public ASTBase {
private:
    StructDefAST *StructDef{nullptr}; // 对应的结构体变量定义指针，暂时不支持，留空
    std::string VarName;              // 变量名
    KType VarType;                    // 变量类型
    Value *Val{nullptr};              // 对应的llvmalue
public:
    VarDefAST(const LineNo&, const std::string&, KType);

   oid setStructDefAST(StructDefAST *sdef) { StructDef = sdef; }
   oid setVarName(const std::string& name) { VarName = name; }
   oid setVarType(KType type)              { VarType = type; }
   oid setLLVMValue(Value *v)              { Val =; }
    
    StructDefAST *getStructDefAST() { return StructDef; }
    std::string getVarName()        { return VarName; }
    Value *getLLVMValue()           { return Val; }
    KType getVarType()              { return VarType; }
};  


/// ------------------------------------------------------------------------
/// @brief ForStmt AST express for statement in kaleidoscope
/// ------------------------------------------------------------------------
class ForStmtAST : public ASTBase {
private:
    ASTBase *Expr1;
    ASTBase *Expr2;
    ASTBase *Expr3;
public:
    ForStmtAST(const LineNo&, ASTBase*, ASTBase*, ASTBase*);
    ForStmtAST(const LineNo&);

   oid setExpr1(ASTBase *expr) { Expr1 = expr; }
   oid setExpr2(ASTBase *expr) { Expr2 = expr; }
   oid setExpr3(ASTBase *expr) { Expr3 = expr; }
   oid setStatement(ASTBase *stmt) { addChild(stmt); }

    ASTBase *getExpr1() const { return Expr1; }
    ASTBase *getExpr2() const { return Expr2; }
    ASTBase *getExpr3() const { return Expr3; }
    ASTBase *getStatement() const { 
        if(childEmpty())
            return nullptr;
        return *begin();
    }
};


/// ------------------------------------------------------------------------
/// @brief WhileStmt AST express while statement in kaleidoscope
/// ------------------------------------------------------------------------
class WhileStmtAST : public ASTBase {
private:
    ASTBase *Cond;
public: 
    explicit WhileStmtAST(const LineNo&, ASTBase*);

   oid setCond(ASTBase *cond) { Cond = cond; }
   oid setStatement(ASTBase *stmt) { addChild(stmt); }

    ASTBase* getCond() const { return Cond; }
    ASTBase* getStatement() const {
        if(childEmpty())
            return nullptr;
        return *begin();       
    }
};


/// ------------------------------------------------------------------------
/// @brief IfStmt AST express if statement in kaleidoscope
/// ------------------------------------------------------------------------
class IfStmtAST : public ASTBase {
private:
    ASTBase *Cond;
    ASTBase *Else;
public:
    explicit IfStmtAST(const LineNo&, ASTBase*);

   oid setCond(ASTBase *cond) { Cond = cond; }
   oid setElse(ASTBase *elseStmt) { Else = elseStmt; }
   oid setStatement(ASTBase *stmt) { addChild(stmt); }

    ASTBase *getCond() const { return Cond; }
    ASTBase *getElse() const { return Else; }
    ASTBase *getStatement() const { 
        if(childEmpty())
            return nullptr;
        return *begin();     
    }
};


/// ------------------------------------------------------------------------
/// @brief BinaryExpr AST expr binary expression in kaleidoscope for example
/// BinaryExpr AST can express 1+3, a >= 8, a[1] = 10...
/// ------------------------------------------------------------------------
class BinaryExprAST : public ASTBase {
private:
    Operator Op;            // T <== The operator of the expression
public:
    BinaryExprAST(const LineNo&, Operator, ASTBase*, ASTBase*);
    
    //oid setOperator(Operator op) { Op = op; }
    //oid setLhs(ASTBase *lhs);
    //oid setRhs(ASTBase *rhs);

    /// @brief get the operator enum type of the expr
    /// @return 
    Operator getExprOp() const { return Op; }
    ASTBase *getLhs() const { return getChild(0); }
    ASTBase *getRhs() const { return getChild(1); }
public:
};


/// ------------------------------------------------------------------------
/// @brief Unary Expr AST express the unary expression in kaleiodscope such
/// as '-a', '!b', '!(a>b)' ...
/// ------------------------------------------------------------------------
class UnaryExprAST : public ASTBase {
private:
    Operator Op;

public:
    UnaryExprAST(const LineNo&, Operator, ASTBase*);

    Operator getExprOp() const { return Op; }
    ASTBase *getUnaryExpr() const { return getChild(0); }
};


/// ------------------------------------------------------------------------
/// @brief Literal Expr AST express the str literalalue in kaleidoscope
/// such as "aaa", "bbb", "cds".
/// ------------------------------------------------------------------------
class LiteralExprAST : public ASTBase {
private:
    std::string Str;
public:
    LiteralExprAST(const LineNo&);
    LiteralExprAST(const LineNo&, const std::string&);

   oid appendCharacter(char c) { Str.push_back(c); }

    std::string getStr() const { return Str; }
};


/// ------------------------------------------------------------------------
/// @brief Number Expr AST express the number literat in kaleidoscope, such 
/// as 10, 10.0, true, false...
/// ------------------------------------------------------------------------
class NumberExprAST : public ASTBase {
private:
    KType Type;
    union {
        char C;
        unsigned char Uc;
        short S;
        unsigned short Us;
        int I;
        unsigned int Ui;
        long L;
        unsigned long Ul;
        double D;
        float F;
        bool B;
    } LitValue;

public:
    explicit NumberExprAST(const LineNo&, char);
    explicit NumberExprAST(const LineNo&, unsigned char);
    explicit NumberExprAST(const LineNo&, short);
    explicit NumberExprAST(const LineNo&, unsigned short);
    explicit NumberExprAST(const LineNo&, int);
    explicit NumberExprAST(const LineNo&, unsigned int);
    explicit NumberExprAST(const LineNo&, long);
    explicit NumberExprAST(const LineNo&, unsigned long);
    explicit NumberExprAST(const LineNo&, double);
    explicit NumberExprAST(const LineNo&, float);
    explicit NumberExprAST(const LineNo&, bool);

    KType getType() { return KType; }
    char getCharValue() { return LitValue.C; }
    unsigned char getUCharValue() { return LitValue.Uc; }
    short getShortValue() { return LitValue.S; }
    unsigned short getUShortValue() { return LitValue.Us; }
    int getIntValue() { return LitValue.I; }
    unsigned int getUIntValue() { return LitValue.Ui; }
    long getLongValue() { return LitValue.L; }
    unsigned long getULongValue() { return LitValue.Ul; }
    double getDoubleValue() { return LitValue.D; }
    float getFloatValue() { return LitValue.F; }
    bool getBoolValue() { return LitValue.B; }
}; 


/// ------------------------------------------------------------------------
/// @brief Id Ref AST express the id ref ofariable
/// ------------------------------------------------------------------------
class IdRefAST : public ASTBase {
private:
    const std::string     IdName;             //ar name
    Param                *Param{nullptr}      // param define
    VarDefAST            *VarDef{nullptr};    //ar define
public:
    explicit IdRefAST(const LineNo&, const std::string&);

   oid setVarDef(VarDefAST*ar) { VarDef =ar; }
   oid setParam(ParamAST *param) { Param = param; }

    Value *getLLVMValue();
    ParamAST *getParam() const { return Param; }
    VarDeclAST *getVarDef() const { return VarDef; }
    std::string getIdName() const { return IdName; }
};


/// ------------------------------------------------------------------------
/// @brief Id Indexes Ref express the id indexed ref of arrayariable
/// ------------------------------------------------------------------------
class IdIndexedRefAST : public ASTBase {
private:
    std::vector<ExprAST*> Indexes;            // indexes list
    const std::string     IdName;             //ar name
    Param                *Param{nullptr}      // param define
    VarDefAST            *VarDef{nullptr};    //ar define
public:
    explicit IdIndexedRefAST(const LineNo&, const std::string&);

   oid addIndex(ExprAST *expr) { Indexes.push_back(expr); }
   oid setVarDef(VarDefAST *var) { VarDef =ar; }
   oid setParam(ParamAST *param) { Param = param; }

    Value *getLLVMValue();
    ParamAST *getParam() const { return Param; }
    std::string getIdName() const { return IdName; }
    VarDefAST  *getVarDef() const { return VarDef; }
    const std::vector<ExprAST*>& getIndexes() const { return Indexes; }
};


/// ------------------------------------------------------------------------
/// @brief Call Expr AST express function call expr
/// ------------------------------------------------------------------------
class CallExprAST : public ASTBase {
private:
    FuncAST *TheCallFunction{nullptr};          // 对应的LLVM Function指针
    const std::string FuncName;                 // 函数名
    std::vector<ExprAST*> ParamList;            // 参数列表
public:
    explicit CallExprAST(const LineNo&, const std::string&);

   oid setLLVMFunction(FuncAST *func) { this->TheCallFunction = func; }
   oid addParam(ExprAST *param) { this->ParamList.push_back(param); }

    const std::vector<ExprAST*> getParamList() const { return this->ParamList; }
    llvm::Function *getLLVMFunction();
    const std::string& getName()               const { return this->FuncName; }
    bool isParamListEmpty()                    const { return this->ParamList.empty(); }
    
};


#endif

