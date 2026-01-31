#include "stdlib.h"
#include <strings.h>
#include <execinfo.h>
#include <unistd.h>


int main() {
    int n = 100;
    void *ptr[n] = {};
    int sizes[n] = {};
    bzero(ptr, n * sizeof(void *));
    for (int i= 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            sizes[j] = random() % 100000; 
            ptr[j] = malloc(sizes[j]);
            bzero(ptr[j], sizes[j]);
        }
        for (int j = 0; j < n; j++) {
            if (ptr[j])
                free(ptr[j]);
            else {
                write(2, "NULL\n", 5);
            }
        }
    }
}
