#include "ast.h"


ASTBase::ASTBase(const LineNo& lineNo) : LineMsg(lineNo), 
                                         Parent(nullptr), 
                                         Program(nullptr){}
                                        
void ASTBase::accept(AstVisitor *v) {
    v->visit(this);
}

void ASTBase::addChild(ASTBase *child) {
    this->Childs.push_back(child);
}

void ASTBase::setParent(ASTBase *parent) {
    this->Parent = parent;
}

void ASTBase::setLineNo(ASTBase *lineNo) {
    this->LineMsg = lineNo;
}

void ASTBase::setProgram(ProgramAST *prog) {
    this->Program = prog;
}

bool ASTBase::childEmpty() {
    return this->Childs.empty();
}

const ASTBase::NodeIter& ASTBase::cbegin() {
    return this->Childs->cbegin();
}

const ASTBase::NodeIter& ASTBase::cend() {
    return this->Childs->cend();
}

const ASTBase::NodeIter& ASTBase::crbegin() {
    return this->Childs->crbegin();
}

const ASTBase::NodeIter& ASTBase::crend() {
    return this->Childs->crend();
}

ASTBase::NodeIter begin() {
    return this->Childs->begin();
}

ASTBase::NodeIter end() {
    return this->Childs->end();
}

ASTBase::NodeIter rbegin() {
    return this->Childs->rbegin();
}

ASTBase::NodeIter rend() {
    return this->Childs->rend();
}



