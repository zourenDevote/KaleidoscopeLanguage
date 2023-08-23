
#ifndef KALE_AST_DUMPER_H
#define KALE_AST_DUMPER_H

#include "ast_visitor.h"

namespace kale {

class DumpVisitor : public AstVisitor {

public:
    DumpVisitor() = default;


    void preAction(ASTBase *node) override;
    void postAction(ASTBase *node) override;
};

}

#endif
