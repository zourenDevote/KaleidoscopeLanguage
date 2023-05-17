
#ifndef KALEIDOSCOPE_IR_GEN
#define KALEIDOSCOPE_IR_GEN

#include "ast.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Type.h"


using namespace llvm;

/// @brief 这个函数遍历AST生成IR代码
class IRGenerator {
private:
    LLVMContext TheContext;
    Module *TheModule;
    IRBuilder<> *TheBuilder;
    Type *IntType;
    Type *BoolType;
    Type *DoubleType;
    Type *VoidType;
    Type *IntPtrType;
    Type *BoolPtrType;
    Type *DoublePtrType;
    ConstantInt *TrueConst;
    ConstantInt *FalseConst;

    // llvm eval value
    Value *CalcValue;
    Function *CurFunction;

    bool isGlobal{false};
    bool notLoad;
    void init();
public:
    IRGenerator(const std::string& modName = "output.ll");
    ~IRGenerator();

    Module *getLLVMModule() { return TheModule; }

    void visit(ProgramAST *node);
    void visit(FuncDefAST *node);
    void visit(FuncExternAST *node);
    void visit(VarDefAST *node);
    void visit(VarExternAST *node);
//    void visit(ParamAST *node);
    void visit(StatementAST *node);
    void visit(ExprStmtAST *node);
    void visit(AssignStmtAST *node);
    void visit(ReturnStmtAST *node);
    void visit(BlockStmtAST *node);
    void visit(IfStmtAST *node);
    void visit(ForStmtAST *node);
    void visit(WhileStmtAST *node);

    void visit(ExprAST *node);
    void visit(BinExprAST *node);
    void visit(UnaryExprAST *node);
    void visit(IdRefAST *node);
    void visit(CallExprAST *node);
    void visit(DoubleExprAST *node);
    void visit(IntExprAST *node);
    void visit(BoolExprAST *node);
};

#endif
