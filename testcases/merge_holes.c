#include "../mems.h"
#include <stdio.h>
#include <stdlib.h>

/*
testcase 1: merge_holes.c
mems.h should be able to merge consecutive holes in the free list
*/
int main()
{
    mems_init();

    int *ptr[10];
    for (int i = 0; i < 10; i++)
    {
        ptr[i] = (int *)mems_malloc(sizeof(int) * 250);
    }

    mems_free(ptr[0]);
    mems_free(ptr[1]);
    mems_free(ptr[3]);
    mems_free(ptr[2]);

    mems_print_stats();
    mems_finish();

    return 0;
}