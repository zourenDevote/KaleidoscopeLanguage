
#include "ast.h"
#include <cassert>

#ifndef KALE_CAST_H
#define KALE_CAST_H

namespace kale {

template<class Dest, class Source> 
Dest *kale_cast(Source *node) {
    if(Dest::canCastTo(node->getClassId())) {
        return dynamic_cast<Dest*>(node);
    }
    return nullptr;
}

}

#endif




