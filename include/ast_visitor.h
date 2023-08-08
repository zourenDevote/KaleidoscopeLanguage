
/**
 * @author: devote
 * @date:   2023-07-16
 * @overview: This file is the visitor defined to visit the ast node.
*/


class ASTBase;
class ProgramAST;
class ParamAST;
class FuncAST;
class InitializedAST;
class StructDefAST;
class VarDefAST;
class BlockStmtAST;
class ReturnStmtAST;
class BreakStmtAST;
class ContinueStmtAST;
class ForStmtAST;
class WhileStmtAST;
class IfStmtAST;
class BinaryExprAST;
class UnaryExprAST;
class LiteralExprAST;
class NumberExprAST;
class IdRefAST;
class IdIndexedRefAST;
class CallExprAST;


#ifndef ADD_VISITOR
#define ADD_VISITOR(X) void visit(X *node);
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
    ADD_VISITOR(VarDefAST)
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

public:
    virtual void preAction(ASTBase *node)   = 0;
    virtual void postAction(ASTBase *node)  = 0;
};


#undef ADD_VISITOR
#endif








