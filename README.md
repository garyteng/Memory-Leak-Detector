# Memory-Leak-Detector
Keep track of every memory allocations

## Goal
* Keep track of every memory allocation
* Modify malloc(), realloc(), free()
* Generate information when program is done

## How to run?
* Execute the following commands
```
make
./memtest
cat result.txt

make clean
```

## How to use?
* You only need to include "mini_valgrind.h" in your test file(test.c as an example).

<pre>
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
</pre>

* Following output will be generated in result.txt file
<pre>
Heap report:

    Leak file test.c : line 28
    Leak size : 500 bytes
    Leak memory address : 0x69a638

    Leak file test.c : line 25
    Leak size : 200 bytes
    Leak memory address : 0x69a4c8


Program made 1 bad call(s) to free.

Heap summary:

    Total memory usage: 810 bytes, Total memory free: 110 bytes
    Total leak: 700 bytes

</pre>
