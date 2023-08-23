
#include "kale_util.h"
#include "cast.h"

namespace kale {

bool KaleUtils::isConstant(ExprAST *expr) {

    switch (expr->getClassId())
    {
        case NumberId: return true;
        case UnaryExprId: {
            UnaryExprAST *unary = kale_cast<UnaryExprAST>(expr);
            return isConstant(unary->getUnaryExpr());
        }
        case BinExprId: {
            BinaryExprAST *bin = kale_cast<BinaryExprAST>(expr);
            return isConstant(bin->getLhs()) && isConstant(bin->getRhs());
        }
        default:
            return false;
    }
}

}




