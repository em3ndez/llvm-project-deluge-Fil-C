#include <stdfil.h>

int main(void)
{
    unsigned x = 666;
    
    asm volatile("add %0, %2" : "+r"(x) : "r"(x) : "cc");

    ZASSERT(x == 666 * 2);

    return 0;
}
