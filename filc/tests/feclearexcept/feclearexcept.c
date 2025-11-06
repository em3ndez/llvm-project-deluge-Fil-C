#define _GNU_SOURCE

#include <fenv.h>

int main()
{
    feclearexcept(FE_INVALID | FE_DIVBYZERO | FE_OVERFLOW | FE_UNDERFLOW);
#ifdef __USE_GNU
    feenableexcept(FE_INVALID | FE_DIVBYZERO | FE_OVERFLOW);
#endif
    return 0;
}

