
#include "ir_support.h"
#include "global_variable.h"
#include "llvm/IR/Constants.h"


namespace kale {

// clang-format off

llvm::Type *KaleIRTypeSupport::KaleVoidType = nullptr;
llvm::Type *KaleIRTypeSupport::KaleBoolType = nullptr;
llvm::Type *KaleIRTypeSupport::KaleCharType = nullptr;
llvm::Type *KaleIRTypeSupport::KaleUCharType = nullptr;
llvm::Type *KaleIRTypeSupport::KaleShortType = nullptr;
llvm::Type *KaleIRTypeSupport::KaleUShortType = nullptr;
llvm::Type *KaleIRTypeSupport::KaleIntType = nullptr;
llvm::Type *KaleIRTypeSupport::KaleUIntType = nullptr;
llvm::Type *KaleIRTypeSupport::KaleLongType = nullptr;
llvm::Type *KaleIRTypeSupport::KaleULongType = nullptr;
llvm::Type *KaleIRTypeSupport::KaleFloatType = nullptr;
llvm::Type *KaleIRTypeSupport::KaleDoubleType = nullptr;

void KaleIRTypeSupport::initIRTypeSupport() {
    KaleVoidType    = llvm::Type::getVoidTy(GlobalContext);
    KaleBoolType    = llvm::Type::getInt1Ty(GlobalContext);
    KaleCharType    = llvm::Type::getInt8Ty(GlobalContext);
    KaleUCharType   = llvm::Type::getInt8Ty(GlobalContext);
    KaleShortType   = llvm::Type::getInt16Ty(GlobalContext);
    KaleUShortType  = llvm::Type::getInt16Ty(GlobalContext);
    KaleIntType     = llvm::Type::getInt32Ty(GlobalContext);
    KaleUIntType    = llvm::Type::getInt32Ty(GlobalContext);
    KaleLongType    = llvm::Type::getInt64Ty(GlobalContext);
    KaleULongType   = llvm::Type::getInt64Ty(GlobalContext);
    KaleFloatType   = llvm::Type::getFloatTy(GlobalContext);
    KaleDoubleType  = llvm::Type::getDoubleTy(GlobalContext);
}

llvm::Constant *KaleIRConstantValueSupport::KaleFalse = nullptr;
llvm::Constant *KaleIRConstantValueSupport::KaleTrue = nullptr;
llvm::Constant *KaleIRConstantValueSupport::KaleCharZero = nullptr;
llvm::Constant *KaleIRConstantValueSupport::KaleUCharZero = nullptr;
llvm::Constant *KaleIRConstantValueSupport::KaleShortZero = nullptr;
llvm::Constant *KaleIRConstantValueSupport::KaleUShortZero = nullptr;
llvm::Constant *KaleIRConstantValueSupport::KaleIntZero = nullptr;
llvm::Constant *KaleIRConstantValueSupport::KaleUIntZero = nullptr;
llvm::Constant *KaleIRConstantValueSupport::KaleLongZero = nullptr;
llvm::Constant *KaleIRConstantValueSupport::KaleULongZero = nullptr;
llvm::Constant *KaleIRConstantValueSupport::KaleFloatZero = nullptr;
llvm::Constant *KaleIRConstantValueSupport::KaleDoubleZero = nullptr;

void KaleIRConstantValueSupport::initIRConastantSupport() {
    KaleTrue        = llvm::ConstantInt::get(KaleIRTypeSupport::KaleBoolType, 1);
    KaleFalse       = llvm::ConstantInt::get(KaleIRTypeSupport::KaleBoolType, 0);
    KaleCharZero    = llvm::ConstantInt::get(KaleIRTypeSupport::KaleCharType, 0);
    KaleUCharZero   = llvm::ConstantInt::get(KaleIRTypeSupport::KaleUCharType, 0);
    KaleShortZero   = llvm::ConstantInt::get(KaleIRTypeSupport::KaleShortType, 0);
    KaleUShortZero  = llvm::ConstantInt::get(KaleIRTypeSupport::KaleUShortType, 0);
    KaleIntZero     = llvm::ConstantInt::get(KaleIRTypeSupport::KaleIntType, 0);
    KaleUIntZero    = llvm::ConstantInt::get(KaleIRTypeSupport::KaleUIntType, 0);
    KaleLongZero    = llvm::ConstantInt::get(KaleIRTypeSupport::KaleLongType, 0);
    KaleULongZero   = llvm::ConstantInt::get(KaleIRTypeSupport::KaleULongType, 0);
    KaleFloatZero   = llvm::ConstantFP::get(KaleIRTypeSupport::KaleFloatType, 0.0);
    KaleDoubleZero  = llvm::ConstantFP::get(KaleIRTypeSupport::KaleDoubleType, 0.0);
}

//clang-format on

}
