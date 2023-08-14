
#include "ast_visitor.h"
#include "ast.h"

#define TraversNode(X) if(X) { X->accept(*this); }

#define TraversArray(X) for(auto node : X) { node->accept(*this); }

void AstVisitor::visit(ASTBase *node) {
    preAction(node);
    postAction(node);
}

void AstVisitor::visit(ProgramAST *node) {
    preAction(node);
    TraversArray(node->getCompElems());
    postAction(node);
}

void AstVisitor::visit(ParamAST *node) {
    preAction(node);
    TraversNode(node->getId());
    postAction(node);
}

void AstVisitor::visit(FuncAST *node) {
    preAction(node);
    TraversArray(node->getParams());
    TraversNode(node->getBlockStmt());
    postAction(node);
}

void AstVisitor::visit(InitializedAST *node) {
    preAction(node);
    TraversNode(node->getInitExpr());
    TraversNode(node->getExpr());
    postAction(node);
}

void AstVisitor::visit(StructDefAST *node) {
    preAction(node);
    postAction(node);
}   

void AstVisitor::visit(DataDeclAST *node) {
    preAction(node);
    TraversArray(node->getVarDecls());
    postAction(node);
}

void AstVisitor::visit(DataTypeAST *node) {
    preAction(node);
    postAction(node);
}

void AstVisitor::visit(VariableAST *node) {
    preAction(node);
    TraversNode(node->getDataType())
    TraversNode(node->getInitExpr())
    postAction(node);
}

void AstVisitor::visit(BlockStmtAST *node) {
    preAction(node);
    TraversArray(node->getStmts());
    postAction(node);
}

void AstVisitor::visit(ReturnStmtAST *node) {
    preAction(node);
    TraversNode(node->getRetExpr());
    postAction(node);
}

void AstVisitor::visit(BreakStmtAST *node) {
    preAction(node);
    postAction(node);    
}

void AstVisitor::visit(ContinueStmtAST *node) {
    preAction(node);
    postAction(node);
}

void AstVisitor::visit(ForStmtAST *node) {
    preAction(node);
    TraversNode(node->getExpr1());
    TraversNode(node->getExpr2());
    TraversNode(node->getExpr3());
    TraversNode(node->getStatement());
    postAction(node);
}

void AstVisitor::visit(WhileStmtAST *node) {
    preAction(node);
    TraversNode(node->getCond());
    TraversNode(node->getStatement());
    postAction(node);
}

void AstVisitor::visit(IfStmtAST *node) {
    preAction(node);
    TraversNode(node->getCond());
    TraversNode(node->getStatement());
    TraversNode(node->getElse());
    postAction(node);
}

void AstVisitor::visit(BinaryExprAST *node) {
    preAction(node);
    TraversNode(node->getLhs());
    TraversNode(node->getRhs());
    postAction(node);
}

void AstVisitor::visit(UnaryExprAST *node) {
    preAction(node);
    TraversNode(node->getUnaryExpr());
    postAction(node);
}

void AstVisitor::visit(LiteralExprAST *node) {
    preAction(node); 
    postAction(node);
}

void AstVisitor::visit(NumberExprAST *node) {
    preAction(node); 
    postAction(node);   
}

void AstVisitor::visit(IdRefAST *node) {
    preAction(node); 
    postAction(node);
}

void AstVisitor::visit(IdIndexedRefAST *node) {
    preAction(node);
    TraversArray(node->getIndexes())
    postAction(node);
}

void AstVisitor::visit(CallExprAST *node) {
    preAction(node);
    TraversArray(node->getArgs())
    postAction(node);
}

void AstVisitor::visit(ExprStmtAST *node) {
    preAction(node);
    TraversNode(node->getExpr());
    postAction(node);
}

//////////////////////////// accept 实现 //////////////////////////
#ifndef ACCEPT
#define ACCEPT(X) void X::accept(AstVisitor &v) { v.visit(this); }

ACCEPT(ProgramAST)
ACCEPT(ParamAST)
ACCEPT(FuncAST)
ACCEPT(InitializedAST)
ACCEPT(StructDefAST)
ACCEPT(DataDeclAST)
ACCEPT(DataTypeAST)
ACCEPT(VariableAST)
ACCEPT(BlockStmtAST)
ACCEPT(ReturnStmtAST)
ACCEPT(BreakStmtAST)
ACCEPT(ContinueStmtAST)
ACCEPT(ForStmtAST)
ACCEPT(WhileStmtAST)
ACCEPT(IfStmtAST)
ACCEPT(BinaryExprAST)
ACCEPT(UnaryExprAST)
ACCEPT(LiteralExprAST)
ACCEPT(NumberExprAST)
ACCEPT(IdRefAST)
ACCEPT(IdIndexedRefAST)
ACCEPT(CallExprAST)
ACCEPT(ExprStmtAST)


#undef ACCEPT
#endif


