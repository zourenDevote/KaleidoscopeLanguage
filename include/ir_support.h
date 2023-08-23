
#ifndef KALE_IR_SUPPORT_H
#define KALE_IR_SUPPORT_H


#include "llvm/IR/Type.h"
#include "llvm/IR/Constant.h"

namespace kale {

class KaleIRTypeSupport {

public:
    static llvm::Type *KaleVoidType;
    static llvm::Type *KaleBoolType;
    static llvm::Type *KaleCharType;
    static llvm::Type *KaleUCharType;
    static llvm::Type *KaleShortType;
    static llvm::Type *KaleUShortType;
    static llvm::Type *KaleIntType;
    static llvm::Type *KaleUIntType;
    static llvm::Type *KaleLongType;
    static llvm::Type *KaleULongType;
    static llvm::Type *KaleFloatType;
    static llvm::Type *KaleDoubleType;

    static void initIRTypeSupport();
};


class KaleIRConstantValueSupport {

public:
    static llvm::Constant *KaleTrue;
    static llvm::Constant *KaleFalse;
    static llvm::Constant *KaleCharZero;
    static llvm::Constant *KaleUCharZero;
    static llvm::Constant *KaleShortZero;
    static llvm::Constant *KaleUShortZero;
    static llvm::Constant *KaleIntZero;
    static llvm::Constant *KaleUIntZero;
    static llvm::Constant *KaleLongZero;
    static llvm::Constant *KaleULongZero;
    static llvm::Constant *KaleFloatZero;
    static llvm::Constant *KaleDoubleZero;

    static void initIRConastantSupport();
};

}

#endif
