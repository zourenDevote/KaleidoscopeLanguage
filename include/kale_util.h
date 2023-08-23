

#ifndef KALE_UTIL_H
#define KALE_UTIL_H

#include "ast.h"

namespace kale {

/**
 * @brief KaleUtils class give api to help compiler generate IR.
*/
class KaleUtils {

public:
    static bool isConstant(ExprAST *expr);

};

}

#endif
