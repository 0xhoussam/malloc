#include <stddef.h>
#include "malloc.h"

int main() {
    char *a = malloc(10);
    char *b = malloc(10);
    char *c = malloc(10);

    free(a);
    free(b);
    free(c);
}
