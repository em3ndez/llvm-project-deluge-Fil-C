#include <fcntl.h>
#include <stdfil.h>
#include <stdlib.h>
#include <string.h>

int main()
{
    char* p = malloc(16);
    free(p);
    strcpy(p, "filc/tests/openfail9/test.txt");  // Use after free
    open(p, O_RDONLY);
    return 0;
}