#include <stdfil.h>

int main()
{
    ZASSERT(zunsafe_call("foo", 42, 666) == 708);
    return 0;
}

