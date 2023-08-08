
#include "ast_visitor.h"
#include "ast.h"

#define TraversNode(X) X->accept(*this);

#define TraversArray(X) for(auto node : X) { node->accept(*this); }

void AstVisitor::visit(ASTBase *node) {
    TraversArray(node->getChilds());
}


void AstVisitor::visit(ProgramAST *node) {
    visit((ASTBase*)node);
}

void AstVisitor::visit(ParamAST *node) {
    visit((ASTBase*)node);
}

void AstVisitor::visit(FuncAST *node) {
    TraversArray(node->getParams());
    visit((ASTBase*)node);
}

void AstVisitor::visit(InitializedAST *node) {

}

void AstVisitor::visit(StructDefAST *node) {
}

void AstVisitor::visit(VarDefAST *node) {

}

void AstVisitor::visit(BlockStmtAST *node) {
    
}

void AstVisitor::visit(ReturnStmtAST *node) {
    
}

void AstVisitor::visit(BreakStmtAST *node) {
    
}

void AstVisitor::visit(ContinueStmtAST *node) {
    
}

void AstVisitor::visit(ForStmtAST *node) {
    
}

void AstVisitor::visit(WhileStmtAST *node) {
    
}

void AstVisitor::visit(IfStmtAST *node) {
    
}

void AstVisitor::visit(BinaryExprAST *node) {
    
}

void AstVisitor::visit(UnaryExprAST *node) {
    
}

void AstVisitor::visit(LiteralExprAST *node) {
    
}

void AstVisitor::visit(NumberExprAST *node) {
    
}

void AstVisitor::visit(IdRefAST *node) {
    
}

void AstVisitor::visit(IdIndexedRefAST *node) {
    
}

void AstVisitor::visit(CallExprAST *node) {
    
}



//////////////////////////// accept 实现 //////////////////////////
#ifndef ACCEPT
#define ACCEPT(X) void X::accept(AstVisitor &v) { v.visit(this); }

ACCEPT(ProgramAST)
ACCEPT(ParamAST)
ACCEPT(FuncAST)
ACCEPT(InitializedAST)
ACCEPT(StructDefAST)
ACCEPT(VarDefAST)
ACCEPT(BlockStmtAST)
ACCEPT(ReturnStmtAST)
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


#undef ACCEPT
#endif


