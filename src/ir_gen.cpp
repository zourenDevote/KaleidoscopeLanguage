
#include "ir_gen.h"

const char IfBodyName[] = "if.body";
const char ElseBodyName[] = "else.body";
const char ForBodyName[] = "for.body";
const char ForCondName[] = "for.cond";
const char WhileCondName[] = "while.cond";
const char WhileBodyName[] = "while.body";
const char AfterBBName[] = "after";

/// 多维数组转化为一维数组
int calcuArraySize(const std::vector<int>& ArrayDim) {
    int len = 0;
    auto iter = ArrayDim.crbegin();
    auto end = ArrayDim.crend();
    while(iter != end) {
        if(len == 0) {
            len = *iter;
        }
        else {
            len *= *iter;
        }
        iter++; 
    }
    return len;
}

IRGenerator::IRGenerator(const std::string& modName) : TheContext() {
    TheModule = new Module(modName.c_str(), TheContext);
    TheBuilder = new IRBuilder<>(TheContext);
    init();
}

IRGenerator::~IRGenerator() {
    delete TheModule;
    delete TheBuilder;
}

void IRGenerator::init() {
    IntType = Type::getInt32Ty(TheContext);
    BoolType = Type::getInt1Ty(TheContext);
    DoubleType = Type::getDoubleTy(TheContext);
    VoidType = Type::getVoidTy(TheContext);
    IntPtrType = Type::getInt32PtrTy(TheContext);
    BoolPtrType = Type::getInt1PtrTy(TheContext);
    DoublePtrType = Type::getDoublePtrTy(TheContext);
    TrueConst = ConstantInt::getTrue(TheContext);
    FalseConst = ConstantInt::getFalse(TheContext);
}


void IRGenerator::visit(ProgramAST *node) {

    // 处理全局变量定义
    for(auto *vardef : node->GlobalDefs) {
        isGlobal = true;
        visit(vardef);
        isGlobal = false;
    }

    // 处理全局变量引用
    for(auto *varextern : node->VarExterns) {
        visit(varextern);
    }

    // 处理函数引用
    for(auto *funcextern : node->FuncExterns) {
        visit(funcextern);
    }

    // 处理函数定义
    for(auto *funcdef : node->FuncDefs) {
        visit(funcdef);
    }

    return;
}

void IRGenerator::visit(VarDefAST *node) {
    Type *varTy = nullptr;
    
    if(node->isBoolType()) {
        varTy = BoolType;
    }
    else if(node->isIntType()) {
        varTy = IntType;
    }
    else {
        varTy = DoubleType;
    }

    if(isGlobal) {
        for(auto *decl : node->VarDecls) {
            if(decl->ArrayDim.size() != 0) {
                int len = calcuArraySize(decl->ArrayDim);
                Constant *init = nullptr;
                if(node->isDoubleType()) {
                    init = ConstantFP::get(varTy, 0.0);
                }
                else {
                    init = ConstantInt::get(varTy, 0);
                }
                ArrayType *arrayTy = ArrayType::get(varTy, len);
                GlobalVariable *gloArray = new GlobalVariable(*TheModule, arrayTy, false, 
                    GlobalVariable::ExternalLinkage, ConstantArray::get(arrayTy, init), decl->VarName.c_str());
                decl->setLLVMValue(gloArray);
            }
            else {
                GlobalVariable *gloVar = new GlobalVariable(*TheModule, varTy, false, GlobalVariable::ExternalLinkage,
                nullptr, decl->VarName.c_str());
                decl->setLLVMValue(gloVar);
            }
        }
    }
    else {
        for(auto *decl : node->VarDecls) {
            if(decl->ArrayDim.size() != 0) {
                int len = calcuArraySize(decl->ArrayDim);
                Type* arrayType = ArrayType::get(varTy, len);
                AllocaInst* intArray = TheBuilder->CreateAlloca(arrayType, nullptr, "myArray");
                decl->setLLVMValue(intArray);
            }
            else {
                AllocaInst* var = TheBuilder->CreateAlloca(varTy, nullptr, decl->VarName.c_str());
                decl->setLLVMValue(var);
            }
        }  
    }
}

void IRGenerator::visit(VarExternAST *node) {
    Type *varTy = nullptr;
    
    if(node->isBoolType()) {
        varTy = BoolType;
    }
    else if(node->isIntType()) {
        varTy = IntType;
    }
    else {
        varTy = DoubleType;
    }


    for(auto *decl : node->VarDecls) {
        if(decl->ArrayDim.size() != 0) {
            int len = calcuArraySize(decl->ArrayDim);
            ArrayType *arrayTy = ArrayType::get(varTy, len);
            GlobalVariable *gloArray = new GlobalVariable(*TheModule, arrayTy, false, 
                    GlobalVariable::ExternalLinkage, nullptr, decl->VarName.c_str());
            decl->setLLVMValue(gloArray);
        }
        else {
            GlobalVariable *gloVar = new GlobalVariable(*TheModule, varTy, false, GlobalVariable::ExternalLinkage,
                nullptr, decl->VarName.c_str());
            decl->setLLVMValue(gloVar);
        }
    }

}

void IRGenerator::visit(FuncExternAST *node) {
    Type *retTy = nullptr;
    switch(node->RetTy) {
        case tok_int : {
            retTy = IntType;
            break;
        }
        case tok_bool : {
            retTy = BoolType;
            break;
        }
        case tok_double : {
            retTy = DoubleType;
            break;
        }
        default : retTy = VoidType; break;
    }

    std::vector<Type*> params; 
    for(auto *param : node->ParamLists) {
        if(param->isIntType()) {
            if(param->ArraySize) {params.push_back(IntPtrType);}
            else {params.push_back(IntType);}
        }
        else if(param->isBoolType()) {
            if(param->ArraySize) {params.push_back(BoolPtrType);}
            else {params.push_back(BoolType);}
        }
        else if(param->isDoubleType()) {
            if(param->ArraySize) {params.push_back(DoublePtrType);}
            else {params.push_back(DoubleType);}
        }
    }

    FunctionType* externFuncType = FunctionType::get(retTy, params, false);

    // 创建外部函数声明
    Function* externFunc = Function::Create(externFuncType, Function::ExternalLinkage, node->FuncName, *TheModule);

    node->setLLVMFunction(externFunc);
}

void IRGenerator::visit(FuncDefAST *node) {
    Type *retTy = nullptr;
    switch(node->RetTy) {
        case tok_int : {
            retTy = IntType;
            break;
        }
        case tok_bool : {
            retTy = BoolType;
            break;
        }
        case tok_double : {
            retTy = DoubleType;
            break;
        }
        default : retTy = VoidType; break;
    }

    std::vector<Type*> params; 
    for(auto *param : node->ParamLists) {
        if(param->isIntType()) {
            if(param->ArraySize) {params.push_back(IntPtrType);}
            else {params.push_back(IntType);}
        }
        else if(param->isBoolType()) {
            if(param->ArraySize) {params.push_back(BoolPtrType);}
            else {params.push_back(BoolType);}
        }
        else if(param->isDoubleType()) {
            if(param->ArraySize) {params.push_back(DoublePtrType);}
            else {params.push_back(DoubleType);}
        }
    }

    FunctionType* FuncType = FunctionType::get(retTy, params, false);

    // 创建外部函数声明
    CurFunction = Function::Create(FuncType, Function::ExternalLinkage, node->FuncName, *TheModule);

    int i = 0;
    for (auto arg = CurFunction->arg_begin(); arg != CurFunction->arg_end(); ++arg) {
        Argument* argument = &*arg;
        node->ParamLists[i++]->setLLVMValue(argument);
    }
    
    node->setLLVMFunction(CurFunction);

    BasicBlock* entryBB = BasicBlock::Create(TheContext, "entry", CurFunction);
    TheBuilder->SetInsertPoint(entryBB);
    visit(node->Statements);

    // 获取函数的基本块列表
    BasicBlock* lastBB = &CurFunction->getBasicBlockList().back();

    // 检查最后一个基本块是否以ReturnInst指令结尾
    if (lastBB->getTerminator()->getOpcode() != Instruction::Ret) {
        TheBuilder->CreateRetVoid();
    }
}


void IRGenerator::visit(StatementAST *node){
    switch(node->stmtId()) {
        case IfStmtId : {
            visit((IfStmtAST*)node);
            break;
        }
        case AssignStmtId : {
            visit((AssignStmtAST*)node);
            break;            
        }
        case ReturnStmtId : {
            visit((ReturnStmtAST*)node);
            break;   
        }
        case ExprStmtId : {
            visit((ExprStmtAST*)node);
            break;
        }
        case ForStmtId : {
            visit((ForStmtAST*)node);
            break;
        }
        case WhileStmtId : {
            visit((WhileStmtAST*)node);
            break;
        }
        case VarDefId : {
            visit((VarDefAST*)node);
            break;
        }
        case BlockStmtId : {
            visit((BlockStmtAST*)node);
            break;            
        }
        default : break;
    }
}

void IRGenerator::visit(ExprStmtAST *node){
    visit(node->Expr);
}

void IRGenerator::visit(AssignStmtAST *node){
    notLoad = true;
    visit(node->Id);
    notLoad = false;
    Value* lhs = CalcValue;
    visit(node->Expr);
    CalcValue = TheBuilder->CreateStore(CalcValue, lhs);
}

void IRGenerator::visit(ReturnStmtAST *node){
    if(node->Expr) {
        visit(node->Expr);
        TheBuilder->CreateRet(CalcValue);
    }
    else {
        TheBuilder->CreateRetVoid();
    }
}

void IRGenerator::visit(BlockStmtAST *node){
    for(auto *stmt : node->Statements) {
        visit(stmt);
    }
}

void IRGenerator::visit(IfStmtAST *node){
    visit(node->Cond);
    BasicBlock *IfBody = BasicBlock::Create(TheContext, IfBodyName, CurFunction);
    BasicBlock *After = BasicBlock::Create(TheContext, AfterBBName, CurFunction);
    if(node->ElseStatement) {
        BasicBlock *ElseBody = BasicBlock::Create(TheContext, ElseBodyName, CurFunction);
        TheBuilder->CreateCondBr(CalcValue, IfBody, ElseBody);
        TheBuilder->SetInsertPoint(IfBody);
        visit(node->Statement);
        TheBuilder->CreateBr(After);
        TheBuilder->SetInsertPoint(ElseBody);
        visit(node->ElseStatement);
        TheBuilder->CreateBr(After);
    }
    else {
        TheBuilder->CreateCondBr(CalcValue, IfBody, After);
        TheBuilder->SetInsertPoint(IfBody);
        visit(node->Statement);
        TheBuilder->CreateBr(After);
    }
}

void IRGenerator::visit(ForStmtAST *node){
    BasicBlock *ForCond = BasicBlock::Create(TheContext, ForCondName, CurFunction);
    BasicBlock *ForBody = BasicBlock::Create(TheContext, ForBodyName, CurFunction);
    BasicBlock *After = BasicBlock::Create(TheContext, AfterBBName, CurFunction);

    if(node->Id) {
        notLoad = true;
        visit(node->Id);
        notLoad = false;
        Value *lhs = CalcValue;
        visit(node->AssignExpr);
        TheBuilder->CreateStore(CalcValue, lhs);
    }

    TheBuilder->CreateBr(ForCond);
    TheBuilder->SetInsertPoint(ForCond);
    visit(node->CondExpr);
    TheBuilder->CreateCondBr(CalcValue, ForBody, After);

    TheBuilder->SetInsertPoint(ForBody);
    visit(node->Statement);

    if(node->Id && node->StepExpr) {
        visit(node->Id);
        Value *lhs = CalcValue;
        visit(node->StepExpr);
        Value *res = nullptr;
        /// @fixme 这里需要类型转换
        // if(v->getType()->isIntegerTy()) {
            res = TheBuilder->CreateNSWAdd(lhs, CalcValue);
        // }
        // else {
        //     res = TheBuilder->CreateFAdd(v, CalcValue);
        // }
        notLoad = true;
        visit(node->Id);
        notLoad = false;
        TheBuilder->CreateStore(res, CalcValue);
    }

    TheBuilder->CreateBr(ForCond);

    TheBuilder->SetInsertPoint(After);
}   

void IRGenerator::visit(WhileStmtAST *node){
    BasicBlock *WhileCond = BasicBlock::Create(TheContext, WhileCondName, CurFunction);
    BasicBlock *WhileBody = BasicBlock::Create(TheContext, WhileBodyName, CurFunction);
    BasicBlock *After = BasicBlock::Create(TheContext, AfterBBName, CurFunction);

    TheBuilder->CreateBr(WhileCond);
    TheBuilder->SetInsertPoint(WhileCond);
    visit(node->Cond);
    TheBuilder->CreateCondBr(CalcValue, WhileBody, After);

    TheBuilder->SetInsertPoint(WhileBody);
    visit(node->Statement);
    TheBuilder->CreateBr(WhileCond);

    TheBuilder->SetInsertPoint(After);
}

void IRGenerator::visit(ExprAST *node) {
    switch (node->exprId())
    {
        case BinExprId : {
            visit((BinExprAST*)node);
            break;
        }
        case UnaryExprId : {
            visit((UnaryExprAST*)node);
            break;
        }
        case IdRefExprId : {
            visit((IdRefAST*)node);
            break;
        }
        case CallExprId : {
            visit((CallExprAST*)node);
            break;
        }
        case DoubleExprId : {
            visit((DoubleExprAST*)node);
            break;
        }
        case IntExprId : {
            visit((IntExprAST*)node);
            break;
        }
        case BoolExprId : {
            visit((BoolExprAST*)node);
            break;
        }
    }
}

void IRGenerator::visit(BinExprAST *node) {
    visit(node->Lhs);
    Value* lhs = CalcValue;
    visit(node->Rhs);

    bool isDouble = false;
    // 整数浮点类型转换
    if(lhs->getType() != CalcValue->getType()) {
        if(lhs->getType()->isDoubleTy()) {
            CalcValue = TheBuilder->CreateSIToFP(CalcValue, DoubleType);
        }
        else if(CalcValue->getType()->isDoubleTy()) {
            lhs = TheBuilder->CreateSIToFP(lhs, DoubleType);
        }
        else if (lhs->getType()->isIntegerTy(32)) {
            CalcValue = TheBuilder->CreateZExt(CalcValue, IntType);
        }
        else if (CalcValue->getType()->isIntegerTy(32)) {
            lhs = TheBuilder->CreateZExt(lhs, IntType);
        }
    }

    if(CalcValue->getType()->isDoubleTy()) {
        isDouble = true;
    }

    switch (node->Op)
    {
        case Plus : {
            if(isDouble) {
                CalcValue = TheBuilder->CreateFAdd(lhs, CalcValue);
            }
            else {
                CalcValue = TheBuilder->CreateNSWAdd(lhs, CalcValue);
            }
            break;
        }
        case Sub : {
            if(isDouble) {
                CalcValue = TheBuilder->CreateFSub(lhs, CalcValue);
            }
            else {
                CalcValue = TheBuilder->CreateNSWSub(lhs, CalcValue);
            }
            break;
        }
        case Mul : {
            if(isDouble) {
                CalcValue = TheBuilder->CreateFMul(lhs, CalcValue);
            }
            else {
                CalcValue = TheBuilder->CreateNSWMul(lhs, CalcValue);
            }
            break;
        }
        case Div : {
            if(isDouble) {
                CalcValue = TheBuilder->CreateFDiv(lhs, CalcValue);
            }
            else {
                CalcValue = TheBuilder->CreateSDiv(lhs, CalcValue);
            }
            break;
        }
        case Neq:
        case Eq : {
            if(isDouble) {
                CalcValue = TheBuilder->CreateFSub(lhs, CalcValue);
                CalcValue = TheBuilder->CreateBitCast(CalcValue, BoolType);
            }
            else {
                CalcValue = TheBuilder->CreateNSWSub(lhs, CalcValue);
                CalcValue = TheBuilder->CreateTrunc(CalcValue, BoolType);
            }
            break;
        }
        case Lt : {
            if(isDouble) {
                CalcValue = TheBuilder->CreateFCmpOLT(lhs, CalcValue);
            }
            else {
                CalcValue = TheBuilder->CreateICmpSLT(lhs, CalcValue);
            }
            break;
        }
        case LtEq : {
            if(isDouble) {
                CalcValue = TheBuilder->CreateFCmpOLE(lhs, CalcValue);
            }
            else {
                CalcValue = TheBuilder->CreateICmpSLE(lhs, CalcValue);
            }
            break;
        }

        case Gt : {
            if(isDouble) {
                CalcValue = TheBuilder->CreateFCmpOGT(lhs, CalcValue);
            }
            else {
                CalcValue = TheBuilder->CreateICmpSGT(lhs, CalcValue);
            }
            break;
        }
        case GtEq : {
            if(isDouble) {
                CalcValue = TheBuilder->CreateFCmpOGE(lhs, CalcValue);
            }
            else {
                CalcValue = TheBuilder->CreateICmpSGE(lhs, CalcValue);
            }
            break;
        }
        default : break;
    }
}

void IRGenerator::visit(UnaryExprAST *node) {
    visit(node->Expr);
    // switch (node->Op)
    // {
    //     case Sub : {
    //         break;
    //     }
    //     case Not : {

    //     }
    // }
}

void IRGenerator::visit(IdRefAST *node) {
    
    Type* ptrTy = nullptr, *ldTy = nullptr;
    Value* ptr;
    Token ty;
    if(node->Decl) {
        ptr = node->Decl->LLVMVal;
        ty = node->Decl->Type;
    }
    else {
        ptr = node->Param->Val;
        ty = node->Param->Type;
    }

    switch(ty) {
        case tok_bool : {
            ptrTy = BoolPtrType;
            ldTy = BoolType;
            break;
        }
        case tok_double : {
            ptrTy = DoublePtrType;
            ldTy = DoubleType;
            break;
        }
        case tok_int : {
            ptrTy = IntPtrType;
            ldTy = IntType;
            break;
        }
    }

    if(node->ArrayIndexes.empty()) {
        if(node->Decl) {
            if(notLoad) {
                CalcValue = node->Decl->LLVMVal;
            }
            else {
                CalcValue = TheBuilder->CreateLoad(ldTy, node->Decl->LLVMVal);
            }
        }
        else {
            CalcValue = node->Param->Val;
        }
    }
    else {
        // 计算array index

        auto iter = node->ArrayIndexes.begin();
        auto end = node->ArrayIndexes.end();
        // std::vector<int>& ArrayDim;
        // int index;
        // if(node->Param) {
        //     ArrayDim = node->Param->ArrayDim;
        //     index = ArrayDim.size() - 1;
        // }
        // else {
        //     ArrayDim = node->Decl->ArrayDim;
        //     index = ArrayDim.size() - 2;
        // }
        std::vector<Value*> indexes;
        bool load = notLoad;
        notLoad = false;
        while(iter != end) {
            visit(*iter);
            indexes.push_back(CalcValue);
            iter++;
        }
        notLoad = load;
        // 根据偏移量获得数组
        CalcValue = TheBuilder->CreateGEP(nullptr, ptr, makeArrayRef(indexes));
        CalcValue = TheBuilder->CreateBitCast(CalcValue, ptrTy);
        if(!notLoad) {
            CalcValue = TheBuilder->CreateLoad(ldTy, CalcValue);
        }
    }
}

void IRGenerator::visit(CallExprAST *node) {
    std::vector<Value*> args;
    for(auto* param : node->Params) {
        visit(param);
        args.push_back(CalcValue);
    }
    ArrayRef<llvm::Value*> argsRef = llvm::makeArrayRef(args);
    if(node->FuncDef) {
        CalcValue = TheBuilder->CreateCall(node->FuncDef->LLVMFunction, argsRef);    
    }
    else {
        CalcValue = TheBuilder->CreateCall(node->FuncExtern->LLVMFunction, argsRef);
    }   
}

void IRGenerator::visit(DoubleExprAST *node) {
    CalcValue = llvm::ConstantFP::get(DoubleType, node->Val);
}

void IRGenerator::visit(IntExprAST *node) {
    CalcValue = llvm::ConstantInt::get(IntType, node->Val);
}

void IRGenerator::visit(BoolExprAST *node) {
    if(node->Val) {
        CalcValue = TrueConst;
    }
    else {
        CalcValue = FalseConst;
    }
    
}


