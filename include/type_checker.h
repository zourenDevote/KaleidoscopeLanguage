//
// Created by 20580 on 2023/9/7.
//

#ifndef KALEIDSCOPE_TYPE_CHECKER_H
#define KALEIDSCOPE_TYPE_CHECKER_H

#include "ast_visitor.h"
#include "common.h"

namespace kale {
    class TypeChecker : public AstVisitor {
    public:
        void visit(kale::BinaryExprAST   *node) override;
        void visit(kale::CallExprAST     *node) override;
        void visit(kale::UnaryExprAST    *node) override;
        void visit(kale::NumberExprAST   *node) override;
        void visit(kale::IdRefAST        *node) override;
        void visit(kale::IdIndexedRefAST *node) override;


        static unsigned getTypeSize(KType t);
        static bool matchType(ExprAST *, ExprAST *);
        static bool isBool(ExprAST *);
        static bool isInt(ExprAST *);
        static bool isFP(ExprAST *);
        static bool isConstant(ExprAST *);
    public:
        static bool isSigned(KType);

    };
}



#endif //KALEIDSCOPE_TYPE_CHECKER_H
