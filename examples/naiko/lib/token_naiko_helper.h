#pragma once

#include "compiler_types.h"

namespace aiko::naiko::helper
{

    void resolveNaiko(Token&);

    NaikoKeyword resolveKeyword(Token&);
    NaikoSymbol resolveSymbol(Token&);
    NaikoOperation resolveOperation(Token&);
    NaikoType resolveType(Token&);

}
