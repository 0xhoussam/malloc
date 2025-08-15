// #include <stddef.h>
#include "malloc.h"
#include <stdio.h>

int main() {
    char *a = malloc(10);
    char *b = malloc(10);
    char *c = malloc(10);
    printf("%p\n", a);

    free(a);
    a = malloc(11);
    printf("%p\n", a);
    free(a);
    a = malloc(10);
    printf("%p\n", a);
    free(a);
    free(b);
    free(c);
}
