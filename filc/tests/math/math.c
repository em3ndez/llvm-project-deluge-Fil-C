#include <math.h>
#include <stdfil.h>
#include "utils.h"

int main()
{
    ZASSERT(sin(0.) == 0.);
    __builtin_set_flt_rounds((int)opaque((void*)__builtin_flt_rounds()));
    return 0;
}

