
#ifndef KALE_COMMON_H
#define KALE_COMMON_H

namespace kale {

/// ------------------------------------------------------------------------
/// @brief The enum type enum the ast type of kaleidoscope language's
/// ast node used in ASTBase, Each type of ast node has its own unique id.
/// ------------------------------------------------------------------------
enum KAstId {
    ProgramId,          /* This type express program                                        */
    DataDeclId,         /* This type express var define and var declare                     */
    VariableId,         /* This type express variable in kaleidoscope                       */
    IdDefId,            /* This type express id base class in kaleidoscope                  */
    DataTypeId,         /* This type expression data type in kaleidoscope                   */
    TypeRefId,          /* This type expression data ref in kaleidoscope                    */
    FuncId,             /* This type express function define and function declare           */
    FuncParamId,        /* This type express function param define                          */
    StmtId,             /* This type express base statement of statement                    */
    ExprStmtId,         /* This type express expr statement of expr statement               */
    SwitchId,           /* This type express switch statement                               */
    IfStmtId,           /* This type express if statement                                   */
    ForStmtId,          /* This type express for statement                                  */
    WhileStmtId,        /* This type express while statement                                */
    InitializeId,       /* This type express var or struct object initialized expression    */
    ReturnStmtId,       /* This type express return statement                               */
    BreakStmtId,        /* This type express break statement                                */
    BlockStmtId,        /* This type express block statement                                */
    ContinueStmtId,     /* This type express continue statement                             */

    ExprId,             /* This type express base expr of expression                        */
    BinExprId,          /* This type express bin expr                                       */
    UnaryExprId,        /* This type express unary expr                                     */
    LiteralId,          /* This type express literal                                        */
    NumberId,           /* This type express number literal                                 */
    IdRefId,            /* This type express id ref                                         */
    IdIndexedRefId,     /* This type express id indexed ref                                 */
    CallId,             /* This type express function call                                  */

    /* Low priority */
    LambdaId,           /* This type express lambda expression                              */
    StructId,           /* This type express struct type define                             */
};


/// ------------------------------------------------------------------------
/// @brief The enum type enum the ast type of kaleidoscope language's
/// ast node used in ASTBase, Each type of ast node has its own unique id.
/// ------------------------------------------------------------------------
enum KType {
    Void,               /* void type of kabeidoscope ----------> 0 byte */
    Double,             /* double type of kabeidoscope --------> 8 byte */
    Float,              /* float type of kabeidoscope ---------> 4 byte */
    Bool,               /* bool type of kabeidoscope ----------> 1 byte */
    Char,               /* char type of kabeidoscope ----------> 1 byte */
    UChar,              /* unsigned char type of kabeidoscope -> 1 byte */
    Enum,               /* enum type of kabeidoscope ----------> 8 byte */           
    Short,              /* short type of kabeidoscope ---------> 2 byte */
    UShort,             /* unsigned short type of kabeidoscope > 2 byte */
    Int,                /* int type of kabeidoscope -----------> 4 byte */
    Uint,               /* unsigned int type of kabeidoscope --> 4 byte */
    Long,               /* long type of kabeidoscope ----------> 8 byte */
    ULong,              /* unsigned long  type of kabeidoscope > 8 byte */
   
    /* Low priority */
    Struct,             /* struct type of kabeidoscope --------> n byte */
    Pointer,            /* pointer type of kabeidoscope -------> 8 byte */
};


/// ------------------------------------------------------------------------
/// @brief The enum type enum the ast type of kaleidoscope language's
/// ast node used in ASTBase, Each type of ast node has its own unique id.
/// ------------------------------------------------------------------------
enum Operator {
    Add,                /* operator +   */
    Sub,                /* operator -   */
    Mul,                /* operator *   */
    Div,                /* operator /   */
    Assign,             /* operator =   */
    Eq,                 /* operator ==  */
    Neq,                /* operator !=  */
    Dot,                /* operator .   */
    Gt,                 /* operator >   */
    Ge,                 /* operator >=  */
    Lt,                 /* operator <   */
    Le,                 /* operator <=  */
    Not,                /* operator !   */
    Rsft,               /* operator >>  */
    Rusft,              /* operator >>> */
    Lsft,               /* operator <<  */
    Lusft,              /* operator <<< */
    Or,                 /* operator ||  */
    And,                /* operator &&  */
    BitOr,              /* operator |   */
    BitAnd,             /* operator &   */
    BitXor,             /* operator ^   */
};

enum KaleOptLevel {
    O0,
    O1,
    O2,
    O3
};

#define INSERT_ACCEPT void accept(AstVisitor &v) override; 
#define INSERT_ENUM(X) KAstId getClassId() override { return X; } \
                       static KAstId classId() { return X; }
#define INSERT_ENUM_NAME(X) const char* getAstName() override {return #X;}

#define ADD_VISITOR_OVERRIDE(X) void visit(X *node) override;

}

#endif 
