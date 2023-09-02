
#include "ir_builder.h"
#include "ir_support.h"
#include "global_variable.h"
#include "llvm/IR/DataLayout.h"
#include "llvm/Support/Host.h"
#include "llvm/ADT/Triple.h"
#include "cast.h"
#include "ast.h"

namespace kale {

#define ENTRY_BBLK "entry"

KaleIRBuilder::KaleIRBuilder(ProgramAST *prog) : Prog(prog) {
    assert(prog && "program can not be nullptr");
    std::string module_name = "module" + std::to_string(prog->getLineNo()->FileIndex);
    TheModule = new llvm::Module(module_name, GlobalContext);
    TheModule->setTargetTriple(llvm::sys::getDefaultTargetTriple());
    TheIRBuilder = new llvm::IRBuilder<>(GlobalContext);
    CurBblk = nullptr;
}

void KaleIRBuilder::generateProgToIr() {
    for(auto *prog : Prog->getDependentProgs()) {
        switch (prog->getCompiledFlag())
        {
            case ProgramAST::CompiledFlag::Success: {
                break;
            }
            case ProgramAST::CompiledFlag::NotCompiled: {
                auto builder = getOrCreateIrBuilderByProg(prog);
                builder->generateProgToIr();
                break;
            }
            default: {
                assert(false && "should'n reach here");
            }
        }
    }
    Prog->accept(*this);
    Prog->setCompiledFlag(ProgramAST::CompiledFlag::Success);
}

void KaleIRBuilder::visit(FuncAST *node) {
    llvm::FunctionType *funcTy = getFunctionTypeByFuncASTNode(node);
    if(node->isFuncDeclare()) {
       node->setLLVMFunction(llvm::dyn_cast<llvm::Function>(TheModule->getOrInsertFunction(node->getFuncName(), funcTy).getCallee()));
    }
    else {
        createAndSetCurrentFunc(node->getFuncName(), funcTy);
        unsigned index = 0;
        for(auto *param : node->getParams()) {
            param->getId()->setLLVMValue(CurFunc->getArg(index++));            
        }
        createAndSetCurrentBblk(ENTRY_BBLK);
        node->getBlockStmt()->accept(*this);
        CurBblk = nullptr;
        CurFunc = nullptr;
    }   
}


void KaleIRBuilder::visit(InitializedAST *node) {
    
}

void KaleIRBuilder::visit(StructDefAST *node) {

}

void KaleIRBuilder::visit(VariableAST *node) {
    llvm::Type *ty = getLLVMType(node->getDataType());
    if(!node->getDims().empty()) {
        long arraysize = 1;
        auto begin = node->getDims().rbegin();
        auto end = node->getDims().rbegin();
        while(begin != end) {
            arraysize *= getConstIntByExpr(*begin);
            begin++;
        }
        ty = llvm::ArrayType::get(ty, arraysize);
    }
    llvm::Value *value;
    if(node->isExtern()) {
        /// extern variable
        value = TheModule->getOrInsertGlobal(node->getName(), ty);
    }   
    else if(node->getParent()->getParent()->getClassId() == ProgramId) {
        /// global variable, 暂时不初始化
        value = new llvm::GlobalVariable(*TheModule, ty, node->isConst(),
            llvm::GlobalVariable::ExternalLinkage, createConstantValue(ty), node->getName());
    }
    else {
        /// value
        llvm::BasicBlock *B = TheIRBuilder->GetInsertBlock();
        TheIRBuilder->SetInsertPoint(&CurFunc->getEntryBlock());
        value = TheIRBuilder->CreateAlloca(ty, nullptr, node->getName());
        TheIRBuilder->SetInsertPoint(B);
    }
    node->setLLVMValue(value);
}

void KaleIRBuilder::visit(ReturnStmtAST *node) {
    if(node->getRetExpr()) {
        node->getRetExpr()->accept(*this);
        TheIRBuilder->CreateRet(LastValue);
    }
    else {
        TheIRBuilder->CreateRetVoid();
    }
}

void KaleIRBuilder::visit(BreakStmtAST *node) {
    assert(!AfterStack.empty() && "break jump stack can not be empty!");
    TheIRBuilder->CreateBr(AfterStack.back());
    llvm::BasicBlock *BreakAfter = llvm::BasicBlock::Create(GlobalContext, "break_next");
    CurFunc->getBasicBlockList().push_back(BreakAfter);
    TheIRBuilder->SetInsertPoint(BreakAfter);
}

void KaleIRBuilder::visit(ContinueStmtAST *node) {
    assert(!CondStack.empty() && "break jump stack can not be empty!");
    TheIRBuilder->CreateBr(CondStack.back());
    llvm::BasicBlock *ContinueAfter = llvm::BasicBlock::Create(GlobalContext, "continue_next");
    CurFunc->getBasicBlockList().push_back(ContinueAfter);
    TheIRBuilder->SetInsertPoint(ContinueAfter);
}

void KaleIRBuilder::visit(ForStmtAST *node) {
    llvm::BasicBlock *Cond = BasicBlock::Create(GlobalContext, "while_cond");
    llvm::BasicBlock *Body = BasicBlock::Create(GlobalContext, "while_body");
    llvm::BasicBlock *After = BasicBlock::Create(GlobalContext, "while_after");

    AfterStack.push_back(After);
    CondStack.push_back(Cond);

    if(node->getExpr1()) node->getExpr1()->accept(*this);

    TheIRBuilder->CreateBr(Cond);
    CurFunc->getBasicBlockList().push_back(Cond);
    TheIRBuilder->SetInsertPoint(Cond);

    LastValue = nullptr;
    if(node->getExpr2()) node->getExpr2()->accept(*this);
    convertToI1();

    TheIRBuilder->CreateCondBr(LastValue, Body, After);
    CurFunc->getBasicBlockList().push_back(Body);
    TheIRBuilder->SetInsertPoint(Body);
    node->getStatement()->accept(*this);
    if(node->getExpr3()) node->getExpr3()->accept(*this);
    TheIRBuilder->CreateBr(Cond);
    CurFunc->getBasicBlockList().push_back(After);
    TheIRBuilder->SetInsertPoint(After);

    AfterStack.pop_back();
    CondStack.pop_back();
}

void KaleIRBuilder::visit(WhileStmtAST *node) {
    llvm::BasicBlock *Cond = BasicBlock::Create(GlobalContext, "while_cond");
    llvm::BasicBlock *Body = BasicBlock::Create(GlobalContext, "while_body");
    llvm::BasicBlock *After = BasicBlock::Create(GlobalContext, "while_after");

    AfterStack.push_back(After);
    CondStack.push_back(Cond);

    TheIRBuilder->CreateBr(Cond);
    CurFunc->getBasicBlockList().push_back(Cond);
    TheIRBuilder->SetInsertPoint(Cond);
    node->getCond()->accept(*this);
    convertToI1();
    TheIRBuilder->CreateCondBr(LastValue, Body, After);
    CurFunc->getBasicBlockList().push_back(Body);
    TheIRBuilder->SetInsertPoint(Body);
    node->getStatement()->accept(*this);
    TheIRBuilder->CreateBr(Cond);
    CurFunc->getBasicBlockList().push_back(After);
    TheIRBuilder->SetInsertPoint(After);

    AfterStack.pop_back();
    CondStack.pop_back();
}

void KaleIRBuilder::visit(IfStmtAST *node) {
    llvm::BasicBlock *IfBody = BasicBlock::Create(GlobalContext, "if_body");
    llvm::BasicBlock *After = BasicBlock::Create(GlobalContext, "if_after");
    node->getCond()->accept(*this);
    convertToI1();
    if(node->getElse()) {
        llvm::BasicBlock *Else = BasicBlock::Create(GlobalContext, "else_body");
        TheIRBuilder->CreateCondBr(LastValue, IfBody, Else);
        CurFunc->getBasicBlockList().push_back(IfBody);
        TheIRBuilder->SetInsertPoint(IfBody);
        node->getStatement()->accept(*this);
        TheIRBuilder->CreateBr(After);

        CurFunc->getBasicBlockList().push_back(Else);
        TheIRBuilder->SetInsertPoint(Else);
        node->getElse()->accept(*this);
        TheIRBuilder->CreateBr(After);

        CurFunc->getBasicBlockList().push_back(After);
        TheIRBuilder->SetInsertPoint(After);
    }
    else {
        TheIRBuilder->CreateCondBr(LastValue, IfBody, After);
        CurFunc->getBasicBlockList().push_back(IfBody);
        TheIRBuilder->SetInsertPoint(IfBody);
        node->getStatement()->accept(*this);
        CurFunc->getBasicBlockList().push_back(After);
        TheIRBuilder->CreateBr(After);
        TheIRBuilder->SetInsertPoint(After);
    }
}

void KaleIRBuilder::visit(BinaryExprAST *node) {
    if(node->getExprOp() == Assign) {
        IsNeedPointer = true;
        node->getLhs()->accept(*this);
        IsNeedPointer = false;
        llvm::Value *lhs = LastValue;
        node->getRhs()->accept(*this);
        storeValueToPointer(lhs, LastValue);
    }
    else {
        node->getLhs()->accept(*this);
        llvm::Value *lhs = LastValue;
        node->getRhs()->accept(*this);
        llvm::Value *rhs = LastValue;
        typeConvert(lhs, rhs);
        switch (node->getExprOp()) {
            case Add: {
                if(lhs->getType()->isFloatTy() || lhs->getType()->isDoubleTy()) {
                    LastValue = TheIRBuilder->CreateFAdd(lhs, rhs);
                }
                else {
                    LastValue = TheIRBuilder->CreateNSWAdd(lhs, rhs);
                }
                break;
            }
            case Sub: {
                if(lhs->getType()->isFloatTy() || lhs->getType()->isDoubleTy()) {
                    LastValue = TheIRBuilder->CreateFSub(lhs, rhs);
                }
                else {
                    LastValue = TheIRBuilder->CreateNSWSub(lhs, rhs);
                }
                break;
            }
            case Mul: {
                if(lhs->getType()->isFloatTy() || lhs->getType()->isDoubleTy()) {
                    LastValue = TheIRBuilder->CreateFMul(lhs, rhs);
                }
                else {
                    LastValue = TheIRBuilder->CreateNSWMul(lhs, rhs);
                }
                break;
            }
            case Div: {
                LastValue = TheIRBuilder->CreateFDiv(lhs, rhs);
                break;
            }
            case Eq: {
                LastValue = TheIRBuilder->CreateICmpEQ(lhs, rhs);
                break;
            }
            case Neq: {
                LastValue = TheIRBuilder->CreateICmpNE(lhs, rhs);
                break;
            }
            case Gt: {
                LastValue = TheIRBuilder->CreateICmpSGT(lhs, rhs);
                break;
            }
            case Ge: {
                LastValue = TheIRBuilder->CreateICmpSGE(lhs, rhs);
                break;
            }
            case Lt: {
                LastValue = TheIRBuilder->CreateICmpSLT(lhs, rhs);
                break;
            }
            case Le: {
                LastValue = TheIRBuilder->CreateICmpSLE(lhs, rhs);
                break;
            }
            case Rsft: {
                LastValue = TheIRBuilder->CreateLShr(lhs, rhs);
                break;
            }
            case Lsft: {
                LastValue = TheIRBuilder->CreateLShr(lhs, rhs);
                break;
            }
            case Or: {
                LastValue = TheIRBuilder->CreateOr(lhs, rhs);
                LastValue = TheIRBuilder->CreateZExtOrBitCast(LastValue, KaleIRTypeSupport::KaleBoolType);
                break;
            }
            case And: {
                LastValue = TheIRBuilder->CreateAnd(lhs, rhs);
                LastValue = TheIRBuilder->CreateZExtOrBitCast(LastValue, KaleIRTypeSupport::KaleBoolType);
                break;
            }
            case BitOr: {
                LastValue = TheIRBuilder->CreateOr(lhs, rhs);
                break;
            }
            case BitAnd: {
                LastValue = TheIRBuilder->CreateAnd(lhs, rhs);
                break;
            }
            case BitXor: {
                LastValue = TheIRBuilder->CreateXor(lhs, rhs);
                break;
            }
            default:
                assert(false && "unsupport attribute");
        }
    }
}

void KaleIRBuilder::visit(UnaryExprAST *node) {
    node->getUnaryExpr()->accept(*this);
    switch (node->getExprOp()) {
        case Not: {
            assert(LastValue->getType()->isIntegerTy() && "error!");
            if(!LastValue->getType()->isIntegerTy(1))
                LastValue = TheIRBuilder->CreateZExt(LastValue, KaleIRTypeSupport::KaleBoolType);
            LastValue = TheIRBuilder->CreateNot(LastValue);
        }
        case Sub: {
            if(LastValue->getType()->isFloatTy()) {
                llvm::Value *zero = ConstantFP::get(LastValue->getType(), 0.0);
                LastValue = TheIRBuilder->CreateFSub(zero, LastValue);
            }
            else {
                llvm::Value *zero = ConstantInt::get(LastValue->getType(), 0);
                LastValue = TheIRBuilder->CreateNSWSub(zero, LastValue);
            }
        }
        case Add: {
            break;
        }
        default: assert(false && "unkonwn operator in unary expr");
    }
}

void KaleIRBuilder::visit(LiteralExprAST *node) {

}

void KaleIRBuilder::visit(NumberExprAST *node) {

    if(node->isLong()) {
        if(node->isSigned()) {
            LastValue = ConstantInt::getSigned(KaleIRTypeSupport::KaleIntType, node->getIValue());
        }
        else {
            LastValue = ConstantInt::get(KaleIRTypeSupport::KaleULongType, node->getUIValue());
        }
    }
    else if(node->isChar()) {
        if(node->isSigned()) {
            LastValue = ConstantInt::getSigned(KaleIRTypeSupport::KaleCharType, node->getCValue());
        }
        else {
            LastValue = ConstantInt::getSigned(KaleIRTypeSupport::KaleUCharType, node->getIValue());
        }
    }
    else if(node->isBoolLiteral()) {
        LastValue = ConstantInt::get(KaleIRTypeSupport::KaleBoolType, node->getBoolValue());
    }
    else {
        LastValue = llvm::ConstantFP::get(KaleIRTypeSupport::KaleDoubleType, node->getFValue());
    }

}

void KaleIRBuilder::visit(IdRefAST *node) {
    NeededType = node->getId()->getDataType()->getDataType();
    if(IsNeedPointer) {
        LastValue = node->getId()->getLLVMValue();
    }
    else {
        LastValue = node->getId()->getLLVMValue();
        LastValue = TheIRBuilder->CreateLoad(LastValue);
    }
}

void KaleIRBuilder::visit(IdIndexedRefAST *node) {

}

void KaleIRBuilder::visit(CallExprAST *node) {
    std::vector<llvm::Value *> args = {};
    auto params = node->getFuncDef()->getParams();
    auto paramargs = node->getArgs();
    unsigned index = 0;
    if(!params.empty()) {
        for(auto param : params) {
            paramargs[index]->accept(*this);
            convertToAimType(kaleTypeToLLVMType(param->getId()->getDataType()->getDataType()));
            args.push_back(LastValue);
        }
    }
    auto callee = node->getLLVMFunction();
    LastValue = TheIRBuilder->CreateCall(callee, args);
}

llvm::FunctionType *KaleIRBuilder::getFunctionTypeByFuncASTNode(FuncAST *node) {
    llvm::Type *rettype = getLLVMType(node->getRetType());
    std::vector<llvm::Type *> argtys;
    for(auto *param : node->getParams()) {
        llvm::Type *ty = getLLVMType(param->getId()->getDataType());
        if(!param->getId()->getDims().empty()) {
            long arraysize = 1;
            auto begin = param->getId()->getDims().rbegin();
            auto end = param->getId()->getDims().rbegin();
            while(begin != end) {
                arraysize *= getConstIntByExpr(*begin);
                begin++;
            }
            ty = llvm::ArrayType::get(ty, arraysize);
            argtys.push_back(ty);
        }
        else {
            argtys.push_back(ty);
        }
    }
    return llvm::FunctionType::get(rettype, argtys, true);
}

llvm::Type *KaleIRBuilder::getLLVMType(DataTypeAST *datatype) {
    switch (datatype->getDataType())
    {
        case Void: return KaleIRTypeSupport::KaleVoidType;
        case Double: return KaleIRTypeSupport::KaleDoubleType;
        case Float: return KaleIRTypeSupport::KaleFloatType;
        case Bool: return KaleIRTypeSupport::KaleBoolType;
        case Char: return KaleIRTypeSupport::KaleCharType;
        case UChar: return KaleIRTypeSupport::KaleUCharType;
        case Short: return KaleIRTypeSupport::KaleShortType;
        case UShort: return KaleIRTypeSupport::KaleUShortType;
        case Int: return KaleIRTypeSupport::KaleIntType;
        case Uint: return KaleIRTypeSupport::KaleUIntType;
        case Long: return KaleIRTypeSupport::KaleLongType;
        case ULong: return KaleIRTypeSupport::KaleULongType;
        case Enum:
        case Struct:
        case Pointer:
            assert(false && "This type have not support yet!");
        default:
            assert(false && "should'n reach here");
    }
}

long KaleIRBuilder::getConstIntByExpr(ExprAST *expr) {
    switch (expr->getClassId())
    {
        case NumberId: {
            NumberExprAST *number = kale_cast<NumberExprAST>(expr);
            return number->getIValue();
        }
        case UnaryExprId: {
            UnaryExprAST *unary = kale_cast<UnaryExprAST>(expr);
            long res = getConstIntByExpr(unary->getUnaryExpr());
            switch(unary->getExprOp()) {
                case Add: return res;
                case Sub: return -res;
                case Not: return !res;
                default: {
                    assert(false && "error operator in unary");
                }
            }
        }
        case BinExprId: {
            BinaryExprAST *bin = kale_cast<BinaryExprAST>(expr);
            long lres = getConstIntByExpr(bin->getLhs());
            long rres = getConstIntByExpr(bin->getRhs());
            switch(bin->getExprOp()) {
                case Add:    return lres + rres;
                case Sub:    return lres - rres;
                case Mul:    return lres * rres;
                case Div:    return lres / rres;
                case Eq:     return lres == rres;
                case Neq:    return lres != rres;
                case Gt:     return lres > rres;
                case Ge:     return lres >= rres;
                case Lt:     return lres < rres;
                case Le:     return lres <= rres;
                case Rsft:   return lres >> rres;
                case Lsft:   return lres << rres;
                case Or:     return lres || rres;
                case And:    return lres && rres;
                case BitOr:  return lres | rres;
                case BitAnd: return lres & rres;
                case BitXor: return lres ^ rres;
                default: {
                    assert(false && "error operator in binary");
                }
            }
        }
        default:{
            assert(false && "Error const expr");
        }
    }
}

void KaleIRBuilder::createAndSetCurrentFunc(const llvm::StringRef& name, llvm::FunctionType *ty) {
    CurFunc = llvm::Function::Create(ty, llvm::GlobalVariable::ExternalLinkage, name, *TheModule);
}
void KaleIRBuilder::createAndSetCurrentBblk(const llvm::StringRef& name) {
    assert(CurFunc);
    llvm::BasicBlock *bblk = llvm::BasicBlock::Create(GlobalContext, name, CurFunc);
    if(CurBblk) {
        if(CurBblk->getTerminator()) {
            CurBblk = bblk;
        }
        else {
            TheIRBuilder->CreateBr(bblk);
        }
    }
    else {
        CurBblk = bblk;
    }
    TheIRBuilder->SetInsertPoint(CurBblk);
}

llvm::Constant *KaleIRBuilder::createConstantValue(llvm::Type *ty) {
    llvm::Constant *constvalue = nullptr;
    return constvalue;
}

void KaleIRBuilder::storeValueToPointer(llvm::Value *lv, llvm::Value *rv) {
    llvm::Type *lhs = lv->getType()->getPointerElementType();
    llvm::Type *rhs = rv->getType();
    if(lhs != rhs) {
        if(lhs->isIntegerTy()) {
            if(rhs->isIntegerTy()) {
                if(lhs->getIntegerBitWidth() > rhs->getIntegerBitWidth()) {
                    rv = TheIRBuilder->CreateZExt(rv, lhs);
                }
                else if(lhs->getIntegerBitWidth() < rhs->getIntegerBitWidth()){
                    rv = TheIRBuilder->CreateTrunc(rv, lhs);
                }
            }
            else {
                rv = TheIRBuilder->CreateFPToUI(rv, lhs);
            }
        }
        else if(lhs->isFloatTy()) {
            if(rhs->isIntegerTy()) {
                rv = TheIRBuilder->CreateUIToFP(rv, lhs);
            }
            else {
                rv = TheIRBuilder->CreateFPTrunc(rv, lhs);
            }
        }
        else if(lhs->isDoubleTy()){
            if(rhs->isIntegerTy()) {
                rv = TheIRBuilder->CreateUIToFP(rv, lhs);
            }
            else {
                rv = TheIRBuilder->CreateFPExt(rv, lhs);
            }
        }
        else {
            assert(false && "error type cast!");
        }
    }
    TheIRBuilder->CreateStore(rv, lv);
}

void KaleIRBuilder::typeConvert(llvm::Value *&lv, llvm::Value *&rv) {
    llvm::Type *rhs = rv->getType();
    llvm::Type *lhs = lv->getType();
    if(rhs == lhs) {
        return;
    }
    else {
        if(lhs->isDoubleTy()) {
            if(rhs->isFloatTy()) rv = TheIRBuilder->CreateFPExt(rv, lhs);
            else if(rhs->isIntegerTy()) rv = TheIRBuilder->CreateUIToFP(rv, lhs);
        }
        else if(lhs->isFloatTy()) {
            if(rhs->isDoubleTy()) lv = TheIRBuilder->CreateFPExt(lv, rhs);
            else rv = TheIRBuilder->CreateUIToFP(rv, lhs);
        }
        else {
            if(rhs->isDoubleTy() || rhs->isFloatTy()) lv = TheIRBuilder->CreateFPExt(lv, rhs);
            else if(lhs->getIntegerBitWidth() > rhs->getIntegerBitWidth()) rv = TheIRBuilder->CreateZExt(rv, lhs);
            else lv = TheIRBuilder->CreateZExt(lv, rhs);
        }
    }
}

void KaleIRBuilder::convertToAimType(llvm::Type *t1) {
    llvm::Type *lty = LastValue->getType();
    if(t1 != lty) {
        if((t1->isFloatTy() || t1->isDoubleTy()) && lty->isIntegerTy()) {
            LastValue = TheIRBuilder->CreateUIToFP(LastValue, t1);
        }
        else if(t1->isIntegerTy() && (lty->isFloatTy() || lty->isDoubleTy())) {
            LastValue = TheIRBuilder->CreateFPToUI(LastValue, t1);
        }
        else {
            if(t1->getIntegerBitWidth() > lty->getIntegerBitWidth())
                LastValue = TheIRBuilder->CreateZExt(LastValue, t1);
            else
                LastValue = TheIRBuilder->CreateTrunc(LastValue, t1);
        }
    }

}

void KaleIRBuilder::convertToI1() {
    if(!LastValue) {
        LastValue = llvm::ConstantInt::get(KaleIRTypeSupport::KaleBoolType, 1);
    }
    llvm::Type *ty = LastValue->getType();
    if(ty->isIntegerTy(1)) return;
    else if(ty->isIntegerTy()) LastValue = TheIRBuilder->CreateTrunc(LastValue, KaleIRTypeSupport::KaleBoolType);
    else { assert(false); }

}

llvm::Type *KaleIRBuilder::kaleTypeToLLVMType(KType ty) {
    switch (ty) {
        case Bool: return KaleIRTypeSupport::KaleBoolType;
        case Char: return KaleIRTypeSupport::KaleCharType;
        case UChar: return KaleIRTypeSupport::KaleUCharType;
        case Short: return KaleIRTypeSupport::KaleShortType;
        case UShort: return KaleIRTypeSupport::KaleUShortType;
        case Int: return KaleIRTypeSupport::KaleIntType;
        case Uint: return KaleIRTypeSupport::KaleUIntType;
        case Long: return KaleIRTypeSupport::KaleLongType;
        case ULong: return KaleIRTypeSupport::KaleULongType;
        case Float: return KaleIRTypeSupport::KaleFloatType;
        case Double: return KaleIRTypeSupport::KaleDoubleType;
        default: { assert(false); }
    }
}

std::unordered_map<ProgramAST *, KaleIRBuilder *> KaleIRBuilder::ProgToIrBuilderMap = {};

KaleIRBuilder *KaleIRBuilder::getOrCreateIrBuilderByProg(ProgramAST *prog) {
    if(ProgToIrBuilderMap.find(prog) == ProgToIrBuilderMap.end()) {
        KaleIRBuilder *builder = new KaleIRBuilder(prog);
        ProgToIrBuilderMap.insert({prog, builder});
        return builder;
    }
    return ProgToIrBuilderMap[prog];
}

}

