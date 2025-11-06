#include <fenv.h>

int main()
{
    feclearexcept(FE_INVALID | FE_DIVBYZERO | FE_OVERFLOW | FE_UNDERFLOW);
    return 0;
}

