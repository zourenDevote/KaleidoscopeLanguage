//
// Created by 20580 on 2023/9/7.
//

#include "type_checker.h"
#include "ast.h"
#include "cast.h"

namespace kale {

    bool TypeChecker::matchType(ExprAST *l, ExprAST *r) {
        if(l->getExprType() == r->getExprType()) {
            return true;
        }
        else if(isFP(l) || isFP(r)) {
            if(l->getExprType() == Double) {r->setExprType(Double); return true;}
            else if(r->getExprType() == Double){l->setExprType(Double); return false;}
            else if(l->getExprType() == Float) {r->setExprType(Float); return true;}
            else {l->setExprType(Float); return false;}
        }
        else {
            unsigned lsize = getTypeSize(l->getExprType());
            unsigned rsize = getTypeSize(r->getExprType());
            if(lsize == rsize) {
                if(l->isSign()) {r->setIsSigned(l->isSign()); r->setExprType(l->getExprType()); return true;}
                else if(r->isSign()) {l->setIsSigned(r->isSign());l->setExprType(r->getExprType()); return false;}
            }
            else if(lsize > rsize) {
                r->setIsSigned(l->isSign()); r->setExprType(l->getExprType()); return true;
            }
            else {
                l->setIsSigned(r->isSign());l->setExprType(r->getExprType()); return false;
            }
        }
    }

    unsigned TypeChecker::getTypeSize(KType t) {
        switch (t) {
            case Bool: return 1;
            case Char: return 8;
            case UChar: return 8;
            case Short: return 16;
            case UShort: return 16;
            case Int: return 32;
            case Uint: return 32;
            case Long: return 64;
            case ULong: return 64;
            case Float: return 32;
            case Double: return 64;
            default:
                assert(false && "un support type");
        }
    }

    bool TypeChecker::isBool(ExprAST *node) {
        return node->getExprType() == Bool;
    }

    bool TypeChecker::isInt(ExprAST *node) {
        switch (node->getExprType()) {
            case Char:
            case UChar:
            case Short:
            case UShort:
            case Int:
            case Uint:
            case Long:
            case ULong:
                return true;
            default:
                return false;
        }
    }


    bool TypeChecker::isFP(ExprAST *node) {
        return node->getExprType() == Double || node->getExprType() == Float;
    }

    bool TypeChecker::isConstant(ExprAST *node) {
        switch (node->getClassId()) {
            case IdRefId: return false;
            case IdIndexedRefId: return false;
            case CallId: return false;
            case NumberId: return true;
            case UnaryExprId: return isConstant(kale_cast<UnaryExprAST>(node)->getUnaryExpr());
            case BinExprId: {
                BinaryExprAST *bin = kale_cast<BinaryExprAST>(node);
                return isConstant(bin->getLhs()) && isConstant(bin->getRhs());
            }
            default: return false;
        }
    }

    bool TypeChecker::isSigned(KType ty) {
        switch (ty) {
            case Char:
            case Short:
            case Int:
            case Long:
            case Double:
            case Float:
                return true;
            case UChar:
            case UShort:
            case Uint:
            case ULong:
                return false;
            default:
                return false;
        }
    }

    void TypeChecker::visit(kale::IdRefAST *node) {
        node->setExprType(node->getId()->getDataType()->getDataType());
        node->setIsSigned(isSigned(node->getExprType()));
    }

    void TypeChecker::visit(kale::IdIndexedRefAST *node) {
        node->setExprType(node->getId()->getDataType()->getDataType());
        node->setIsSigned(isSigned(node->getExprType()));
        AstVisitor::visit(node);
    }

    void TypeChecker::visit(kale::CallExprAST *node) {
        if(node->isCallStd()) {
            if(node->getName() == "Print") {
                node->setExprType(Void);
                node->setIsSigned(false);
            }
            else if(node->getName() == "PrintLn") {
                node->setExprType(Void);
                node->setIsSigned(false);
            }
            else if(node->getName() == "GetInt") {
                node->setExprType(Int);
                node->setIsSigned(true);
            }
            else if(node->getName() == "GetDouble") {
                node->setExprType(Double);
                node->setIsSigned(true);
            }
         }
        else {
            node->setExprType(node->getFuncDef()->getRetType()->getDataType());
            node->setIsSigned(isSigned(node->getExprType()));
        }
        AstVisitor::visit(node);
    }

    void TypeChecker::visit(kale::UnaryExprAST *node) {
        node->getUnaryExpr()->accept(*this);
        node->setExprType(node->getUnaryExpr()->getExprType());
        node->setIsSigned(node->getUnaryExpr()->isSign());
    }

    void TypeChecker::visit(kale::BinaryExprAST *node) {
        node->getLhs()->accept(*this);
        node->getRhs()->accept(*this);
        if(isConstant(node)) {
            if(matchType(node->getLhs(), node->getRhs())) {
                node->setExprType(node->getLhs()->getExprType());
                node->setIsSigned(node->getLhs()->isSign());
            }
            else {
                node->setExprType(node->getRhs()->getExprType());
                node->setIsSigned(node->getRhs()->isSign());
            }
        }
        else if(isConstant(node->getLhs())) {
            node->getLhs()->setExprType(node->getRhs()->getExprType());
            node->getLhs()->setIsSigned(node->getRhs()->isSign());
            node->setExprType(node->getRhs()->getExprType());
            node->setIsSigned(node->getRhs()->isSign());
        }
        else if(isConstant(node->getRhs())) {
            node->getRhs()->setExprType(node->getLhs()->getExprType());
            node->getRhs()->setIsSigned(node->getLhs()->isSign());
            node->setExprType(node->getLhs()->getExprType());
            node->setIsSigned(node->getLhs()->isSign());
        }
        else {
            if(matchType(node->getLhs(), node->getRhs())) {
                node->setExprType(node->getLhs()->getExprType());
                node->setIsSigned(node->getLhs()->isSign());
            }
            else {
                node->setExprType(node->getRhs()->getExprType());
                node->setIsSigned(node->getRhs()->isSign());
            }
        }
    }

    void TypeChecker::visit(kale::NumberExprAST *node) {
        // do nothing
        node->setIsSigned(node->isSigned());
        if(node->isBoolLiteral()) node->setExprType(Bool);
        else if(node->isChar()) {
            if(node->isSigned()) node->setExprType(Char);
            else node->setExprType(UChar);
        }
        else if(node->isLong()) {
            if(node->isSigned()) node->setExprType(Long);
            else node->setExprType(ULong);
        }
        else {
            node->setExprType(Double);
        }
    }
}


