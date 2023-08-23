
/**
 * @author: devote
 * @date:   2023-07-16
 * @overview: This file is the visitor defined to visit the ast node.
*/

#ifndef KALE_AST_VISITOR_H
#define KALE_AST_VISITOR_H

namespace kale {

class ASTBase;
class ProgramAST;
class ParamAST;
class FuncAST;
class InitializedAST;
class StructDefAST;
class VariableAST;
class DataDeclAST;
class DataTypeAST;
class BlockStmtAST;
class ReturnStmtAST;
class BreakStmtAST;
class ContinueStmtAST;
class ForStmtAST;
class WhileStmtAST;
class IfStmtAST;
class ExprAST;
class BinaryExprAST;
class UnaryExprAST;
class LiteralExprAST;
class NumberExprAST;
class IdRefAST;
class IdIndexedRefAST;
class CallExprAST;
class ExprStmtAST;

#define TraversNode(X) X->accept(this);

#define TraversArray(X) for(auto *node : X) { node->accept(this); }

#ifndef ADD_VISITOR
#define ADD_VISITOR(X) virtual void visit(X *node);
/// ------------------------------------------------------------------------
/// @brief AstVisitor this class is the base class to visit the ast node.
/// ------------------------------------------------------------------------
class AstVisitor {

public:
    AstVisitor() = default;

public:
    ADD_VISITOR(ASTBase)
    ADD_VISITOR(ProgramAST)
    ADD_VISITOR(ParamAST)
    ADD_VISITOR(FuncAST)
    ADD_VISITOR(InitializedAST)
    ADD_VISITOR(StructDefAST)
    ADD_VISITOR(DataTypeAST)
    ADD_VISITOR(VariableAST)
    ADD_VISITOR(DataDeclAST)
    ADD_VISITOR(BlockStmtAST)
    ADD_VISITOR(ReturnStmtAST)
    ADD_VISITOR(BreakStmtAST)
    ADD_VISITOR(ContinueStmtAST)
    ADD_VISITOR(ForStmtAST)
    ADD_VISITOR(WhileStmtAST)
    ADD_VISITOR(IfStmtAST)
    ADD_VISITOR(BinaryExprAST)
    ADD_VISITOR(UnaryExprAST)
    ADD_VISITOR(LiteralExprAST)
    ADD_VISITOR(NumberExprAST)
    ADD_VISITOR(IdRefAST)
    ADD_VISITOR(IdIndexedRefAST)
    ADD_VISITOR(CallExprAST)
    ADD_VISITOR(ExprStmtAST)

public:
    virtual void preAction(ASTBase *node) {};
    virtual void postAction(ASTBase *node) {};
};


#undef ADD_VISITOR
#endif

}

#endif







