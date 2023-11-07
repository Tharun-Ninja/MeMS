#include "../mems.h"
#include <stdio.h>
#include <stdlib.h>

/*
testcase 2: phy_addr.c
mems.h should be able to return the correct mapping physical address of the virtual address
*/
int main()
{
    mems_init();

    char *name = (char *)mems_malloc(sizeof(char) * 100);
    printf("vir_addr: %p\n", name);
    name = mems_get(name); // get the physical addr to write into that
    name = "Hello World";

    printf("name: %s\n", name);
    printf("phy_addr: %p\n", name);

    mems_print_stats();
    mems_finish();

    return 0;
}