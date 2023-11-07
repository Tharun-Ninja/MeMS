#include "mems.h"
#include <stdio.h>
#include <stdlib.h>

int main()
{
    mems_init();
    // char *name = (char *)mems_malloc(sizeof(char) * 4096);
    // name = mems_get(name);
    // name = "Hello world";

    int *ptr[0];
    ptr[0] = (int *)mems_malloc(sizeof(int) * 250);
    ptr[0] = (int *)mems_get(ptr[0]);
    // ptr[0] = (int *)malloc(sizeof(int) * 250);

    ptr[0][0] = 132;
    printf("%d\n", ptr[0][0]);

    // printf("\n------- Allocated virtual addresses [mems_malloc] -------\n");
    // for (int i = 0; i < 10; i++)
    // {
    //     ptr[i] = (int *)mems_malloc(sizeof(int) * 250);
    //     // printf("Virtual address: %lu\n", (unsigned long)ptr[i]);
    // }

    // mems_free(ptr[0]);
    // mems_free(ptr[1]);
    // mems_free(ptr[3]);
    // mems_free(ptr[2]);

    // void *phy_addr = mems_get(name);
    // printf("phy_addr: %p\n", phy_addr);

    // char *name2 = (char *)mems_malloc(sizeof(char) * 100);
    mems_print_stats();
    mems_finish();
    mems_print_stats();

    return 0;
}