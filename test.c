/**
 * Mini Valgrind Lab
 * CS 241 - Fall 2016
 */

#include "mini_valgrind.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
    // your tests here using malloc, realloc and free

    char *p = malloc(10);
    strcpy(p, "123456789");
    printf("%s\n", p); // check 
    free(p);           // No memory leak

    p = NULL;
    free(p); // Bad free

    p = malloc(100);             // 100 bytes of memory leak
    p = (char*) realloc(p, 200); // malloc new 200 bytes, and free old 100 btyes
    p = (char*) realloc(p, 50);  // still 200 bytes of memory leaks, nothing changed

    p = malloc(500); // another 500 bytes of memory leak

    atexit(print_report); // generate output file
    return 0;
}
