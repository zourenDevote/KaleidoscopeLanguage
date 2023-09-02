
#ifndef KALE_IR_BUILDER_H
#define KALE_IR_BUILDER_H

#include "common.h"
#include "ast_visitor.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Value.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Verifier.h"
#include <unordered_map>

namespace kale {

class KaleIRBuilder: public AstVisitor {

private:
    llvm::Module        *TheModule;
    llvm::IRBuilder<>   *TheIRBuilder;
    llvm::Function      *CurFunc;
    llvm::BasicBlock    *CurBblk;
    ProgramAST          *Prog;
    bool                 IsRootScope;
    bool                 IsNeedPointer = false;
    llvm::Value         *LastValue;
    KType                NeededType;
    std::vector<llvm::BasicBlock *> AfterStack;
    std::vector<llvm::BasicBlock *> CondStack;
public:
    KaleIRBuilder(ProgramAST *prog);
    void generateProgToIr();    

public:
    llvm::Module *getLLVMModule()  { return TheModule; }
protected:
    ADD_VISITOR_OVERRIDE(FuncAST)
    ADD_VISITOR_OVERRIDE(InitializedAST)
    ADD_VISITOR_OVERRIDE(StructDefAST)
    ADD_VISITOR_OVERRIDE(VariableAST)
    ADD_VISITOR_OVERRIDE(ReturnStmtAST)
    ADD_VISITOR_OVERRIDE(BreakStmtAST)
    ADD_VISITOR_OVERRIDE(ContinueStmtAST)
    ADD_VISITOR_OVERRIDE(ForStmtAST)
    ADD_VISITOR_OVERRIDE(WhileStmtAST)
    ADD_VISITOR_OVERRIDE(IfStmtAST)
    ADD_VISITOR_OVERRIDE(BinaryExprAST)
    ADD_VISITOR_OVERRIDE(UnaryExprAST)
    ADD_VISITOR_OVERRIDE(LiteralExprAST)
    ADD_VISITOR_OVERRIDE(NumberExprAST)
    ADD_VISITOR_OVERRIDE(IdRefAST)
    ADD_VISITOR_OVERRIDE(IdIndexedRefAST)
    ADD_VISITOR_OVERRIDE(CallExprAST)

private:
    llvm::FunctionType *getFunctionTypeByFuncASTNode(FuncAST *);
    llvm::Type         *getLLVMType(DataTypeAST *);
    long                getConstIntByExpr(ExprAST *expr);
    
    void                createAndSetCurrentFunc(const llvm::StringRef& name, llvm::FunctionType *ty);
    void                createAndSetCurrentBblk(const llvm::StringRef& name);
    llvm::Constant     *createConstantValue(llvm::Type *ty);
    void                storeValueToPointer(llvm::Value *lv, llvm::Value *rv);
    void                typeConvert(llvm::Value *&lv, llvm::Value *&rv);
    void                convertToAimType(llvm::Type *t1);
    void                convertToI1();
    llvm::Type         *kaleTypeToLLVMType(KType ty);

private:
    static std::unordered_map<ProgramAST *, KaleIRBuilder *> ProgToIrBuilderMap;

public:
    static KaleIRBuilder *getOrCreateIrBuilderByProg(ProgramAST *prog);
};

}

#endif

