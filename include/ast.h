

#ifndef KALEIDSCOPE_AST
#define KALEIDSCOPE_AST


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
    using CNodeIter = std::vector<ASTBase*>::const_iterator; 
    using RNodeIter = std::vector<ASTBase*>::reverse_iterator;
    using CRNodeIter = std::vector<ASTBase*>::const_reverse_iterator;

    ASTBase(const LineNo&);

   virtual void accept(AstVisitor *v);

   void setId(KAstId id);
   void addChild(ASTBase *child);
   void setParent(ASTBase *parent);
   void setLineNo(const LineNo&);
   void setProgram(ProgramAST *prog);



    bool childEmpty();
    NodeIter begin();
    NodeIter end();
    RNodeIter rbegin();
    RNodeIter rend();
    CNodeIter cbegin();
    CNodeIter cend();
    CRNodeIter crbegin();
    CRNodeIter crend();
    virtual ASTBase* deepCopy() { return nullptr; }
    ASTBase *getChild(unsigned int index);
    LineNo *getLineNo() { return &LineMsg; }
    ASTBase *getParent() { return Parent; }
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
    void addDependentProg(ProgramAST *prog) { this->DependentProg.push_back(prog); }
    /// @brief 获取program的依赖项
    /// @return 
    const std::vector<ProgramAST*>& getDependentProgs() { return this->DependentProg; }
    /// @brief 设置编译状态
    /// @param flag 
    void setCompiledFlag(CompiledFlag flag) { CompFlag = flag; }
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
    Value *Val;
public:
    ParamAST(const LineNo&, const std::string&, KType);

    void addArrayDim(int dim) { ArrayDims.push_back(dim); }
    void arrayDimAdd() { ArrayDim++; }
    void setLLVMValue(Value *v) { Val = v; }

    const std::string& getParamName() { return Name; }
    KType getParamType() { return Type; }
    Value *getLLVMValue() { return Val; }
    const std::vector<int>& getArrayDims() { return ArrayDims; }
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

    void setLLVMFunction(Function *func) { LLVMFunc = func; }
    void setFuncName(const std::string& name) { FuncName = name; }
    void setRetType(KType type) { RetType = type; }
    void addFuncParam(ParamAST *param) { FuncParams.push_back(param); }

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
    InitializedAST(const LineNo&, ASTBase*);

    void setInitExpr(ASTBase *expr) { InitExpr = expr; }
    ASTBase *getInitExpr() { return InitExpr; }
};


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

    void setStructDefAST(StructDefAST *sdef) { StructDef = sdef; }
    void setVarName(const std::string& name) { VarName = name; }
    void setVarType(KType type)              { VarType = type; }
    void setLLVMValue(Value *v)              { Val = v; }
    
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

    void setExpr1(ASTBase *expr) { Expr1 = expr; }
    void setExpr2(ASTBase *expr) { Expr2 = expr; }
    void setExpr3(ASTBase *expr) { Expr3 = expr; }
    void setStatement(ASTBase *stmt) { addChild(stmt); }

    ASTBase *getExpr1() const { return Expr1; }
    ASTBase *getExpr2() const { return Expr2; }
    ASTBase *getExpr3() const { return Expr3; }
    ASTBase *getStatement()  { 
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

    void setCond(ASTBase *cond) { Cond = cond; }
    void setStatement(ASTBase *stmt) { addChild(stmt); }

    ASTBase* getCond() const { return Cond; }
    ASTBase* getStatement() {
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

    void setCond(ASTBase *cond) { Cond = cond; }
    void setElse(ASTBase *elseStmt) { Else = elseStmt; }
    void setStatement(ASTBase *stmt) { addChild(stmt); }

    ASTBase *getCond() const { return Cond; }
    ASTBase *getElse() const { return Else; }
    ASTBase *getStatement() { 
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
    
    // void setOperator(Operator op) { Op = op; }
    // void setLhs(ASTBase *lhs);
    // void setRhs(ASTBase *rhs);

    /// @brief get the operator enum type of the expr
    /// @return 
    Operator getExprOp() const { return Op; }
    ASTBase *getLhs() { return getChild(0); }
    ASTBase *getRhs() { return getChild(1); }
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
    ASTBase *getUnaryExpr() { return getChild(0); }
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

    void appendCharacter(char c) { Str.push_back(c); }

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

    KType getType() { return Type; }
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
    ParamAST             *Param{nullptr};     // param define
    VarDefAST            *VarDef{nullptr};    //ar define
public:
    explicit IdRefAST(const LineNo&, const std::string&);

    void setVarDef(VarDefAST*ar) { VarDef =ar; }
    void setParam(ParamAST *param) { Param = param; }

    Value *getLLVMValue();
    ParamAST *getParam() const { return Param; }
    VarDefAST *getVarDef() const { return VarDef; }
    std::string getIdName() const { return IdName; }
};


/// ------------------------------------------------------------------------
/// @brief Id Indexes Ref express the id indexed ref of arrayariable
/// ------------------------------------------------------------------------
class IdIndexedRefAST : public ASTBase {
private:
    std::vector<ASTBase*> Indexes;             // indexes list
    const std::string     IdName;              //ar name
    ParamAST              *Param{nullptr};     // param define
    VarDefAST             *VarDef{nullptr};    //ar define
public:
    explicit IdIndexedRefAST(const LineNo&, const std::string&);

    void addIndex(ASTBase *expr) { Indexes.push_back(expr); }
    void setVarDef(VarDefAST *var) { VarDef = var; }
    void setParam(ParamAST *param) { Param = param; }

    Value *getLLVMValue();
    ParamAST *getParam() const { return Param; }
    std::string getIdName() const { return IdName; }
    VarDefAST  *getVarDef() const { return VarDef; }
    const std::vector<ASTBase*>& getIndexes() const { return Indexes; }
};


/// ------------------------------------------------------------------------
/// @brief Call Expr AST express function call expr
/// ------------------------------------------------------------------------
class CallExprAST : public ASTBase {
private:
    FuncAST *TheCallFunction{nullptr};          // 对应的LLVM Function指针
    const std::string FuncName;                 // 函数名
    std::vector<ASTBase*> ParamList;            // 参数列表
public:
    explicit CallExprAST(const LineNo&, const std::string&);

    void setLLVMFunction(FuncAST *func) { this->TheCallFunction = func; }
    void addParam(ASTBase *param) { this->ParamList.push_back(param); }

    const std::vector<ASTBase*> getParamList() const { return this->ParamList; }
    llvm::Function *getLLVMFunction();
    const std::string& getName()               const { return this->FuncName; }
    bool isParamListEmpty()                    const { return this->ParamList.empty(); }
    
};


#endif

