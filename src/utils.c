#include "herd.h"

#include <stdio.h>
#include <stdlib.h>

void *malloc_or_die(size_t size) {
    void *mem = malloc(size);
    if (!mem && size) {
        fprintf(stderr, "malloc failed");
        abort();
    }
    return mem;
}
