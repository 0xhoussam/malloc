// #include <stddef.h>
#include "malloc.h"
#include "stdlib.h"
#include <stdio.h>
#include <strings.h>

int main() {

    for (int i= 0; i < 100; i++) {
        int as = random();
        int bs = random();
        int cs = random();

        int *a = malloc(as);
        int *b = malloc(bs);
        int *c = malloc(cs);

        bzero(a, as);
        bzero(b, bs);
        bzero(c, cs);
        
    }
}
