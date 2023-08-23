
#include "ir_builder.h"
#include "ir_support.h"
#include "global_variable.h"
#include "cast.h"
#include "ast.h"

namespace kale {

#define ENTRY_BBLK "entry"

KaleIRBuilder::KaleIRBuilder(ProgramAST *prog) : Prog(prog) {
    assert(prog && "program can not be nullptr");
    std::string module_name = "module" + std::to_string(prog->getLineNo()->FileIndex);
    TheModule = new llvm::Module(module_name, GlobalContext);
    TheIRBuilder = new llvm::IRBuilder<>(GlobalContext);
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
        TheModule->getOrInsertFunction(node->getFuncName(), funcTy);
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
        TheIRBuilder->SetInsertPoint(&(*CurFunc->getEntryBlock().getFirstInsertionPt()));
        value = TheIRBuilder->CreateAlloca(ty, nullptr, node->getName());
        TheIRBuilder->SetInsertPoint(B);
    }
    node->setLLVMValue(value);
}

void KaleIRBuilder::visit(ReturnStmtAST *node) {
    if(node->getRetExpr()) {
        node->getRetExpr()->accept(*this);
    }
    else {
        TheIRBuilder->CreateRetVoid();
    }
}

void KaleIRBuilder::visit(BreakStmtAST *node) {

}

void KaleIRBuilder::visit(ContinueStmtAST *node) {

}

void KaleIRBuilder::visit(ForStmtAST *node) {

}

void KaleIRBuilder::visit(WhileStmtAST *node) {

}

void KaleIRBuilder::visit(IfStmtAST *node) {

}

void KaleIRBuilder::visit(BinaryExprAST *node) {

}

void KaleIRBuilder::visit(UnaryExprAST *node) {

}

void KaleIRBuilder::visit(LiteralExprAST *node) {

}

void KaleIRBuilder::visit(NumberExprAST *node) {
    
}

void KaleIRBuilder::visit(IdRefAST *node) {

}

void KaleIRBuilder::visit(IdIndexedRefAST *node) {

}

void KaleIRBuilder::visit(CallExprAST *node) {

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
            return number->getLongValue();
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
        TheIRBuilder->SetInsertPoint(CurBblk);
    }
    else {
        CurBblk = bblk;
    }
}

llvm::Constant *KaleIRBuilder::createConstantValue(llvm::Type *ty) {
    llvm::Constant *constvalue = nullptr;

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

