// #include <stddef.h>
#include "malloc.h"
#include "stdlib.h"
#include <signal.h>
#include <stdio.h>
#include <strings.h>
#include <execinfo.h>
#include <unistd.h>


int main() {
  // signal(SIGSEGV, handler);
    for (int i= 0; i < 100; i++) {
        int as = random() % 10000;
        // printf("as: %i\n", as);
        int bs = random() % 10000;
        // printf("bs: %i\n", bs);
        int cs = random() % 10000;
        // printf("cs: %i\n", cs);

        write(1, "allocating a\n", 14);
        char *a = malloc(as);
        write(1, "allocating b\n", 14);
        char *b = malloc(bs);
        write(1, "allocating c\n", 14);
        char *c = malloc(cs);

        bzero(a, as);
        bzero(b, bs);
        bzero(c, cs);

        a[0] = 'A';
        b[0] = 'B';
        c[0] = 'C';

        write(1, "free a\n", 8);
        free(a);
        write(1,"free b\n", 8);
        free(b);
        write(1, "free c\n", 8);
        free(c);
        
    }
}
