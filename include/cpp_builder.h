//
// Created by 27259 on 2023/9/15.
//

#ifndef KALECC_CPP_BUILDER_H
#define KALECC_CPP_BUILDER_H

#include <cstring>
#include "common.h"
#include "iostream"
#include "ast_visitor.h"

namespace kale {

    class CppBuilder : public AstVisitor {

    public:
        CppBuilder(std::ostream &Out = std::cout);

    public:
        void preAction(ASTBase *node) override;
        void postAction(ASTBase *node) override;
    public:
        int deepth = 0;
        void write(std::string input);
        void writeEnd();
    private:
        std::ostream &out;
        char buffer[256];
        int size;
        char *head;
    public:
        ADD_VISITOR_OVERRIDE(ASTBase)

        ADD_VISITOR_OVERRIDE(ProgramAST)

        ADD_VISITOR_OVERRIDE(ParamAST)

        ADD_VISITOR_OVERRIDE(FuncAST)

        ADD_VISITOR_OVERRIDE(InitializedAST)

        ADD_VISITOR_OVERRIDE(StructDefAST)

        ADD_VISITOR_OVERRIDE(DataTypeAST)

        ADD_VISITOR_OVERRIDE(VariableAST)

        ADD_VISITOR_OVERRIDE(DataDeclAST)

        ADD_VISITOR_OVERRIDE(BlockStmtAST)

        ADD_VISITOR_OVERRIDE(ReturnStmtAST)

        ADD_VISITOR_OVERRIDE(BreakStmtAST)

        ADD_VISITOR_OVERRIDE(ContinueStmtAST)

        ADD_VISITOR_OVERRIDE(ForStmtAST)

        ADD_VISITOR_OVERRIDE(WhileStmtAST)

        ADD_VISITOR_OVERRIDE(IfStmtAST)

        ADD_VISITOR_OVERRIDE(BinaryExprAST)

        ADD_VISITOR_OVERRIDE(UnaryExprAST)

        ADD_VISITOR_OVERRIDE(LiteralExprAST)

        ADD_VISITOR_OVERRIDE(NumberExprAST)

        ADD_VISITOR_OVERRIDE(IdRefAST)

        ADD_VISITOR_OVERRIDE(IdIndexedRefAST)

        ADD_VISITOR_OVERRIDE(CallExprAST)

        ADD_VISITOR_OVERRIDE(ExprStmtAST)
    };

}

#endif //KALECC_CPP_BUILDER_H
