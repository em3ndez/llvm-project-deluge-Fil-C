#include <stdfil.h>

int main(void)
{
    int x = 5;
    int y;
    asm("leal 0(%1), %0" : "=r"(y) : "r"(x));
    zprintf("y = %d\n", y);
    return 0;
}
