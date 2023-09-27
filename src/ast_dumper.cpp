#include <iostream>
#include "ast.h"
#include "ast_dumper.h"

namespace kale {

    int deepTh = 0;
    const char *astName[] = {
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
    const char *typeName[] = {
            "Void",
            "Double",
            "Float",
            "Bool",
            "Char",
            "UChar",
            "Enum",
            "Short",
            "UShort",
            "Int",
            "Uint",
            "Long",
            "ULong",
            "Struct",
            "Pointer"
    };


    void DumpVisitor::preAction(ASTBase *node) {
        // write you code here;
        for (int i = 0; i < deepTh; i++) {
            if (i == deepTh - 1) {
                std::cout << "|-";
            } else {
                std::cout << " ";
            }
        }

        int astIndex = node->getClassId();
        LineNo *address = node->getLineNo();
        std::string shortAddress = std::to_string(reinterpret_cast<uintptr_t>(node));
        shortAddress = "0x" + shortAddress.substr(shortAddress.length() - 6);
        int typeIndex;
        switch (astIndex) {
            case 2: {
                IdDefAST *variableNode = dynamic_cast<IdDefAST *>(node);
                std::cout << astName[astIndex] << " " << shortAddress << " " << "<line:" << address->Row << ", col:"
                          << address->Col << "> '"
                          << variableNode->getName() << "' "
                          << std::endl;
                break;
            }
            case 4: {
                DataTypeAST *dataTypeNode = dynamic_cast<DataTypeAST *>(node);
                typeIndex = dataTypeNode->getDataType();
                std::cout << astName[astIndex] << " " << shortAddress << " " << "<line:" << address->Row << ", col:"
                          << address->Col << "> '"
                          << typeName[typeIndex] << "' "
                          << std::endl;
                break;
            }
            case 6: {
                FuncAST *funcNode = dynamic_cast<FuncAST *>(node);
                typeIndex = (funcNode->getRetType())->getDataType();
                std::cout << astName[astIndex] << " " << shortAddress << " " << "<line:" << address->Row << ", col:"
                          << address->Col << ">"
                          << "  '" << typeName[typeIndex] << "' '" << funcNode->getFuncName() << "' "
                          << std::endl;
                break;
            }
            case 24: {
                IdRefAST *idRefNode = dynamic_cast<IdRefAST *>(node);
                std::cout << astName[astIndex] << " " << shortAddress << " " << "<line:" << address->Row << ", col:"
                          << address->Col << "> '"
                          << idRefNode->getIdName() << "'"
                          << std::endl;
                break;
            }
            default: {
                std::cout << astName[astIndex] << " " << shortAddress << " " << "<line:" << address->Row << ", col:"
                          << address->Col << ">" << std::endl;
                break;
            }
        }
        deepTh++;

    }


    void DumpVisitor::postAction(ASTBase *node) {
        // write you code here;
        deepTh--;
    }

}
