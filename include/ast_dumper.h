
#ifndef KALE_AST_DUMPER
#define KALE_AST_DUMPER

#include "ast_visitor.h"

class DumpVisitor : public AstVisitor {

public:
    DumpVisitor() = default;


    void preAction(ASTBase *node) override;
    void postAction(ASTBase *node) override;
};

#endif
