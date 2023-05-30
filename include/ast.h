

#ifndef KALEIDSCOPE_AST
#define KALEIDSCOPE_AST

#include <vector>
#include "common.h"
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
/// message of ast to sources file, and define the virtual function accept
/// to accept the visitor for visit each ast node.
/// ------------------------------------------------------------------------
class ASTBase {
private:
    LineNo LineMsg;
    ASTBase *Parent{nullptr};
    ProgramAST *Program;
    std::vector<ASTBase*> Childs;
    KAstId Id;
public:
    using NodeIter = std::vector<ASTBase*>::iterator; 

    ASTBase(const LineNo& lineNo);

    virtual void accept(AstVisitor *v);

    void setId(KAstId id);
    void addChild(ASTBase *child);
    void setParent(ASTBase *parent);
    void setLineNo(const LineNo& lineNo);
    void setProgram(ProgramAST *prog);


    bool childEmpty();
    NodeIter begin();
    NodeIter end();
    NodeIter rbegin();
    NodeIter rend();
    const NodeIter& cbegin();
    const NodeIter& cend();
    const NodeIter& crbegin();
    const NodeIter& crend();
    virtual ASTBase* deepCopy() { return nullptr; }
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
    unsigned int FileIndex;
    std::vector<ProgramAST*> DependentProg; // 依赖项
    CompiledFlag CompFlag{NotCompiled};     // 处理状态
public:
    ProgramAST() = default;
    /// @brief 添加program的依赖项
    /// @param prog 
    void addDependentProg(ProgramAST *prog) { this->DependensProg.push_back(prog); }
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
/// @brief FuncAST express function define and function delcare in kaleidoscope
/// ------------------------------------------------------------------------
class FuncAST : public ASTBase {
private:
    using ParamIter = std::vector<ParamAST *>::iterator;

    std::string FuncName;                    // 函数名
    std::vector<ParamAST *> FuncParams;      // 函数参数列表
    KType RetType;                           // 返回值类型
public:
    FuncDefAST(const std::string& FuncName, KType retType);

    void setFuncName(const std::string& name) { FuncName = name; }
    void setRetType(KType type) { RetType = type; }
    void addFuncParam(ParamAST *param) { FuncParams.push_back(param); }

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
/// var define. for example in source ode 'int a = 10;' and 'int b[10] = {1, 2, 3}'
/// the initialize ast express 10 and {1,2,3}
/// ------------------------------------------------------------------------
class InitializedAST : public ASTBase {
private:
    /// @brief 初始化表达式
    ASTBase *InitExpr;
public:
    InitializedAST(ExprAST *expr);

    void setInitExpr(ExprAST *expr) { InitExpr = expr; }
    ASTBase *getInitExpr() { return InitExpr; }
}


/// ------------------------------------------------------------------------
/// @brief Struct AST express struct define in kaleidoscope
/// @todo not support in this time
/// ------------------------------------------------------------------------
class StructDefAST : public ASTBase{
public:
    StructDefAST() = default;
};


/// ------------------------------------------------------------------------
/// @brief Var def ast express var define and var extern in kaleidoscope.
/// ------------------------------------------------------------------------
class VarDefAST : public ASTBase {
private:
    StructDefAST *StructDef{nullptr}; // 对应的结构体变量定义指针，暂时不支持，留空
    std::string VarName;              // 变量名
    KType VarType;                    // 变量类型
    Value *Val{nullptr};              // 对应的llvm value
public:
    VarDefAST(const std::string& name, KType type);

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
    ASTBase *Expr1{nullptr};
    ASTBase *Expr2{nullptr};
    ASTBase *Expr3{nullptr};
public:
    ForStmtAST(ASTBase *expr1, ASTBase *expr2, ASTBase *expr3);
    ForStmtAST() = default;

    void setExpr1(ASTBase *expr) { Expr1 = expr; }
    void setExpr2(ASTBase *expr) { Expr2 = expr; }
    void setExpr3(ASTBase *expr) { Expr3 = expr; }
    void setStatement(ASTBase *stmt) { addChild(stmt); }

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
    explicit WhileStmtAST(ASTBase *cond);

    void setCond(ASTBase *cond) { Cond = cond; }
    void setStatement(ASTBase *stmt) { addChild(stmt); }

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
    ASTBase *Else{nullptr};
public:
    explicit IfStmtAST(ASTBase *cond);

    void setCond(ASTBase *cond) { Cond = cond; }
    void setElse(ASTBase *elseStmt) { Else = elseStmt; }
    void setStatement(ASTBase *stmt) { addChild(stmt); }

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
    ASTBase *Lhs;
    ASTBase *Rhs;
    
public:
};


/// ------------------------------------------------------------------------
/// @brief The enum type enum the ast type of kaleidoscope language's
/// ast node used in ASTBase, Each type of ast node has its own unique id.
/// ------------------------------------------------------------------------
class UnartExprAST : public ASTBase {

};


/// ------------------------------------------------------------------------
/// @brief The enum type enum the ast type of kaleidoscope language's
/// ast node used in ASTBase, Each type of ast node has its own unique id.
/// ------------------------------------------------------------------------
class LiteralExprAST : public ASTBase {
    
};

/// ------------------------------------------------------------------------
/// @brief The enum type enum the ast type of kaleidoscope language's
/// ast node used in ASTBase, Each type of ast node has its own unique id.
/// ------------------------------------------------------------------------
class NumberExprAST : public ASTBase {
    KType Type;
} 


/// ------------------------------------------------------------------------
/// @brief The enum type enum the ast type of kaleidoscope language's
/// ast node used in ASTBase, Each type of ast node has its own unique id.
/// ------------------------------------------------------------------------
class IdRefAST : public ASTBase {
private:
    const std::string     IdName;             // 变量名
    VarDefAST *           VarDef{nullptr};    // 变量定义的位置
public:
    explicit IdRefAST(const std::string& name);

    VarDeclAST *getVarDef() const { return VarDef; }
    std::string getIdName() const { return IdName; }
};


/// ------------------------------------------------------------------------
/// @brief The enum type enum the ast type of kaleidoscope language's
/// ast node used in ASTBase, Each type of ast node has its own unique id.
/// ------------------------------------------------------------------------
class IdIndexedRef : public ASTBase {
private:
    std::vector<ExprAST*> Indexes;            // 引用列表
    const std::string     IdName;             // 变量名
    VarDefAST            *VarDef{nullptr};    // 变量定义的位置
public:
    explicit IdIndexedRef(const std::string& name);

    void addIndex(ExprAST *expr) { Indexes.push_back(expr); }
    void setVarDef(VarDefAST *var) { VarDef = var; }

    std::string getIdName() const { return IdName; }
    VarDefAST  *getVarDef() const { return VarDef; }
    const std::vector<ExprAST*>& getIndexes() const { return Indexes; }
};


/// ------------------------------------------------------------------------
/// @brief The enum type enum the ast type of kaleidoscope language's
/// ast node used in ASTBase, Each type of ast node has its own unique id.
/// ------------------------------------------------------------------------
class CallExprAST : public ASTBase {
private:
    llvm::Function *TheCallFunction{nullptr};   // 对应的LLVM Function指针
    const std::string FuncName;                 // 函数名
    std::vector<ExprAST*> ParamList;            // 参数列表
public:
    explicit CallExprAST(const std::string& name);

    void setLLVMFunction(llvm::Function* func) { this->TheCallFunction = func; }
    void addParam(ExprAST *param) { this->ParamList.push_back(param); }

    const std::vector<ExprAST*> getParamList() const { return this->ParamList; }
    llvm::Function *getLLVMFunction()          const { return this->TheCallFunction; }
    const std::string& getName()               const { return this->FuncName; }
    bool isParamListEmpty()                    const { return this->ParamList.empty(); }
    
};


#endif

