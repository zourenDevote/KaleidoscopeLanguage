//
// Created by 27259 on 2023/9/15.
//

#include <iostream>
#include "cpp_builder.h"
#include "ast.h"
#include "cast.h"
#include <unistd.h>
#include <cstdlib>  // 包含头文件以使用 system 函数
/* 1.解决getAstName方法(现在还是老办法）,可以引申到getType方法，想过放在common.h里面但是编译不成功（怎么解决？）
 * 2.解决变量节点 int a; int a = 10; int a = 6+9*4;  （已解决）
 * 3.解决数组问题 int all[5]; all[2] = 4(已解决);
 * 4.解决函数问题，包括参数，以及callId  （已解决）
 * 5.解决for，while循环（已解决）
 * 6.解决if问题 (已解决）
 * 7.解决缩进问题(已解决);
 * */
namespace kale {
#define TraversNode(X) if(X) { X->accept(*this); }
#define TraversArray(X) for(auto node : X) { node->accept(*this); }
    int argNum = 0;
    int parmNum = 0;
    CppBuilder *build;
    const char *astName1[] = {
            "ProgramId",          /* This type express program                                        */
            "DataDeclId",         /* This type express var define and var declare                     */
            "VariableId",         /* This type express variable in kaleidoscope                       */
            "IdDefId",            /* This type express id base class in kaleidoscope                  */
            "DataTypeId",         /* This type expression data type in kaleidoscope                   */
            "TypeRefId",          /* This type expression data ref in kaleidoscope                    */
            "FuncId",             /* This type express function define and function declare           */
            "FuncParamId",        /* This type express function param define                          */
            "StmtId",             /* This type express base statement of statement                    */
            "ExprStmtId",         /* This type express expr statement of expr statement               */
            "SwitchId",           /* This type express switch statement                               */
            "IfStmtId",           /* This type express if statement                                   */
            "ForStmtId",          /* This type express for statement                                  */
            "WhileStmtId",        /* This type express while statement                                */
            "InitializeId",       /* This type express var or struct object initialized expression    */
            "ReturnStmtId",       /* This type express return statement                               */
            "BreakStmtId",        /* This type express break statement                                */
            "BlockStmtId",        /* This type express block statement                                */
            "ContinueStmtId",     /* This type express continue statement                             */
            "ExprId",             /* This type express base expr of expression                        */
            "BinExprId",          /* This type express bin expr                                       */
            "UnaryExprId",        /* This type express unary expr                                     */
            "LiteralId",          /* This type express literal                                        */
            "NumberId",           /* This type express number literal                                 */
            "IdRefId",            /* This type express id ref                                         */
            "IdIndexedRefId",     /* This type express id indexed ref                                 */
            "CallId",             /* This type express function call                                  */
            "LambdaId",           /* This type express lambda expression                              */
            "StructId"            /* This type express struct type define                             */

    };
    const char *typeName1[] = {"Void", "Double", "Float", "Bool", "Char", "UChar", "Enum", "Short", "UShort", "Int",
                               "Uint", "Long", "ULong", "Struct", "Pointer"};

    const char *op[] = {"+",    // Add
                        "-",    // Sub
                        "*",    // Mul
                        "/",    // Div
                        "=",    // Assign
                        "==",   // Eq
                        "!=",   // Neq
                        ".",    // Dot
                        ">",    // Gt
                        ">=",   // Ge
                        "<",    // Lt
                        "<=",   // Le
                        "!",    // Not
                        ">>",   // Rsft
                        ">>>",  // Rusft
                        "<<",   // Lsft
                        "<<<",  // Lusft
                        "||",   // Or
                        "&&",   // And
                        "|",    // BitOr
                        "&",    // BitAnd
                        "^"     // BitXor
    };

    void goodLook() {
        for (int i = 0; i < build->deepth; i++) {
            build->write("\t");
        }
    }
    CppBuilder::CppBuilder(std::ostream &Out) : AstVisitor(), out(Out) {
        this->head = buffer;
        this->size = 0;
        build = this;
    }
    void CppBuilder::write(std::string input) {
        bool isDou = false;
        bool isKua = false;
        for (char c : input) {
            if (head == buffer + sizeof(buffer)) {
                // 缓冲区已满，将其写入输出流
                out.write(buffer, size);
                head = buffer;
                size = 0;
            }
            if (c == ',' && *(head - 1) == ')') {
                // 如果当前字符是','且前一个字符是')'，则替换为')'
                *(head - 1) = ')';
            } else {
                *head++ = c;
                size++;
            }
        }
    }
    void CppBuilder::writeEnd() {
        if(size != 0){
            out.write(buffer,size);
            size = 0;
        }
        out.flush();
    }
    void CppBuilder::visit(ASTBase *node) {

    }

    void CppBuilder::visit(ProgramAST *node) {
        TraversArray(node->getCompElems());
        build->writeEnd();

    }

    void CppBuilder::visit(ParamAST *node) {
        TraversNode(node->getId());
    }

    void CppBuilder::visit(FuncAST *node) {
        TraversNode(node->getRetType())
        build->write(node->getFuncName());
        build->write("(");
        parmNum = node->getParams().size() - 1;
        TraversArray(node->getParams());
        build->write(") {\n");
        build->deepth++;
        TraversNode(node->getBlockStmt());
        build->deepth--;
        build->write ("}\n");
    }

    void CppBuilder::visit(InitializedAST *node) {
        TraversNode(node->getInitExpr());
        TraversNode(node->getExpr());
    }

    void CppBuilder::visit(StructDefAST *node) {

    }

    void CppBuilder::visit(DataDeclAST *node) {
        TraversArray(node->getVarDecls());
    }

    void CppBuilder::visit(DataTypeAST *node) {
        int typeIndex = node->getDataType();
        std::string type = typeName1[typeIndex];
        for (char &c: type) {
            c = std::tolower(c);
            break;
        }
        build->write(type + " ");
    }

    void CppBuilder::visit(VariableAST *node) {
        goodLook();
        if ((node->getParent())->getClassId() == 7) {
            TraversNode(node->getDataType());
            std::string name(node->getName());
            if(parmNum != 0){
                build->write(name + ",");
                parmNum--;
            }else{
                build->write(name);
            }
        } else {
            if (node->isStatic()) {
                build->write ("static ");
            }
            if (node->isExtern()) {
                build->write ("extern ");
            }
            if (node->isConst()) {
                build->write ("const ");
            }
            TraversNode(node->getDataType())
            build->write (node->getName());
            if (node->isArrray()) {
                NumberExprAST *numberNode = kale_cast<NumberExprAST>(node->getDims()[0]);
                std::string length = std::to_string(numberNode->getIValue());
                build->write ("[" + length + "]");
            }
            if (node->hasInitExpr()) {
                build->write (" = ");
                TraversNode(node->getInitExpr())
            }
            build->write (";\n");
        }
    }

    void CppBuilder::visit(BlockStmtAST *node) {
        TraversArray(node->getStmts());
    }

    void CppBuilder::visit(ReturnStmtAST *node) {
        goodLook();
        build->write ("return ");
        TraversNode(node->getRetExpr());
        build->write (";\n");
    }

    void CppBuilder::visit(BreakStmtAST *node) {
        goodLook();
        build->write ("break;\n");
    }

    void CppBuilder::visit(ContinueStmtAST *node) {
        goodLook();
        build->write ("continue;\n");
    }

    void CppBuilder::visit(ForStmtAST *node) {
        goodLook();
        build->write ("for (");
        TraversNode(node->getExpr1());
        build->write (";");
        TraversNode(node->getExpr2());
        build->write (";");
        TraversNode(node->getExpr3());
        build->write (")");
        build->write (" {\n");
        build->deepth++;
        TraversNode(node->getStatement());
        build->deepth--;
        goodLook();
        build->write ("}\n");
    }

    void CppBuilder::visit(WhileStmtAST *node) {
        goodLook();
        build->write ("while (");
        TraversNode(node->getCond());
        build->write (") {\n");
        build->deepth++;
        TraversNode(node->getStatement());
        build->deepth--;
        goodLook();
        build->write ("}\n");
    }

    void CppBuilder::visit(IfStmtAST *node) {
        goodLook();
        build->write ("if (");
        TraversNode(node->getCond());
        build->write (") {\n");
        build->deepth++;
        TraversNode(node->getStatement());
        build->deepth--;
        goodLook();
        build->write ("}\n");
        if (node->getElse()) {
            goodLook();
            build->write ("else");
            build->write (" {\n");
            build->deepth++;
            TraversNode(node->getElse());
            build->deepth--;
            goodLook();
            build->write ("}\n");

        }
    }

    void CppBuilder::visit(BinaryExprAST *node) {
        std::string ope(op[node->getExprOp()]);
        if ((node->getLhs())->getClassId() == 12) {
            for (int i = 0; i < build->deepth; i++) {
                build->write (" ");
            }
            TraversNode(node->getLhs());
            build->write (" " + ope + " ");
            TraversNode(node->getRhs());
            build->write (";\n");
        } else {
            TraversNode(node->getLhs());
            build->write (" " + ope + " ");
            TraversNode(node->getRhs());
        }
    }

    void CppBuilder::visit(UnaryExprAST *node) {
        build->write (op[node->getExprOp()]);
        TraversNode(node->getUnaryExpr());
    }

    void CppBuilder::visit(LiteralExprAST *node) {
        build->write("\"" + node->getStr() + "\"" );
        if ((node->getParent())->getClassId() == 26) {
            if(argNum != 0){
                build->write (",");
                argNum--;
            }
        }
    }

    void CppBuilder::visit(NumberExprAST *node) {
        if (node->isBoolLiteral()) {
            node->getBoolValue() ? build->write("true") : build->write("false");
        } else if (node->isLong()) {
            std::string value = std::to_string(node->getIValue());
            build->write (value);
        } else if (node->isChar()) {
            int c = node->getCValue();
            build->write(c + " ");
        } else if (node->isDouble()) {
            std::string value = std::to_string(node->getFValue());
            build->write (value);
        }
        if ((node->getParent())->getClassId() == 26) {
            if(argNum != 0){
                build->write (",");
                argNum--;
            }
        }
    }

    void CppBuilder::visit(IdRefAST *node) {
        build->write (node->getIdName());
        if ((node->getParent())->getClassId() == 26) {
            if(argNum != 0){
                build->write (",");
                argNum--;
            }
        }
    }

    void CppBuilder::visit(IdIndexedRefAST *node) {
        build->write (node->getIdName());
        build->write ("[");
        TraversArray(node->getIndexes())
        build->write ("]");
    }

    void CppBuilder::visit(CallExprAST *node) {
        build->write (node->getName());
        if (node->isCallStd()) {

        }
        build->write ("(");
        argNum = node->getArgs().size() - 1;
        TraversArray(node->getArgs())
        build->write (")");
    }

    void CppBuilder::visit(ExprStmtAST *node) {
        goodLook();
        TraversNode(node->getExpr());
        build->write(";\n");
    }

    void CppBuilder::preAction(kale::ASTBase *node) {
        int astIndex = node->getClassId();
        std::cout << astName1[astIndex] << std::endl;
    }

    void CppBuilder::postAction(kale::ASTBase *node) {

    }
}