/*
All the main functions with respect to the MeMS are inplemented here
read the function discription for more details

NOTE: DO NOT CHANGE THE NAME OR SIGNATURE OF FUNCTIONS ALREADY PROVIDED
you are only allowed to implement the functions
you can also make additional helper functions a you wish

REFER DOCUMENTATION FOR MORE DETAILS ON FUNSTIONS AND THEIR FUNCTIONALITY
*/
// add other headers as required
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>

/*
Use this macro where ever you need PAGE_SIZE.
As PAGESIZE can differ system to system we should have flexibility to modify this
macro to make the output of all system same and conduct a fair evaluation.
*/
#define PAGE_SIZE 4096

// node that will store the information about the segment
typedef struct sub_node
{
    struct sub_node *next;
    struct sub_node *prev;
    int type;
    void *start_virtual_address;
    void *end_virtual_address;
    int size;
    void *memory;
} sub_node;

typedef struct main_node
{
    struct main_node *next;
    struct main_node *prev;
    void *start_virtual_address;
    void *end_virtual_address;
    sub_node *sub_head;
    sub_node *sub_tail;
    int pages;

} main_node;

// mems_vaddr is the starting virtual address of the heap in our MeMS virtual address space.
void *virtual_addr;
main_node *main_head;
main_node *main_tail;
void *free_list_allocation;
void *current_free_list_allocation;
int remaining_free_list_allocation;

/*
Initializes all the required parameters for the MeMS system. The main parameters to be initialized are:
1. the head of the free list i.e. the pointer that points to the head of the free list
2. the starting MeMS virtual address from which the heap in our MeMS virtual address space will start.
3. any other global variable that you want for the MeMS implementation can be initialized here.
Input Parameter: Nothing
Returns: Nothing
*/
void mems_init()
{
    virtual_addr = 0;
    main_head = NULL;
    main_tail = NULL;
    free_list_allocation = NULL;
    current_free_list_allocation = NULL;
    remaining_free_list_allocation = 0;
    free_list_allocation = mmap(NULL, PAGE_SIZE, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

    if (current_free_list_allocation == MAP_FAILED)
    {
        perror("mmap failed"); // Print an error message if mmap fails
        // Handle the error appropriately, e.g., by returning an error code or exiting.
    }

    current_free_list_allocation = free_list_allocation;
    remaining_free_list_allocation = PAGE_SIZE;
}

/*
This function will be called at the end of the MeMS system and its main job is to unmap the
allocated memory using the munmap system call.
Input Parameter: Nothing
Returns: Nothing
*/
void mems_finish()
{
    main_node *main_loop_pointer = main_head;
    sub_node *sub_loop_pointer = NULL;
    sub_node *next_sub_node = NULL;
    main_node *next_main_node = NULL;

    while (main_loop_pointer != NULL)
    {
        munmap(main_loop_pointer->sub_head->memory, main_loop_pointer->pages * PAGE_SIZE);
        main_loop_pointer = main_loop_pointer->next;
    }

    munmap(free_list_allocation, PAGE_SIZE);

    virtual_addr = 0;
    main_head = NULL;
    main_tail = NULL;
    free_list_allocation = NULL;
    current_free_list_allocation = NULL;
    remaining_free_list_allocation = 0;
    free_list_allocation = NULL;

    printf("-------- Unmapping all memory [mems_finish] -------- \n");
}

sub_node *add_sub_node(int no_pages, main_node *main_node)
{
    if (sizeof(sub_node) > remaining_free_list_allocation)
    {
        free_list_allocation = mmap(NULL, PAGE_SIZE, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
        current_free_list_allocation = free_list_allocation;
        remaining_free_list_allocation = PAGE_SIZE;
    }

    sub_node *new_node = (sub_node *)current_free_list_allocation;
    current_free_list_allocation += sizeof(sub_node);
    remaining_free_list_allocation -= sizeof(sub_node);

    new_node->start_virtual_address = virtual_addr;
    virtual_addr += no_pages * PAGE_SIZE;
    new_node->next = NULL;
    new_node->prev = NULL;
    new_node->type = 0; // Hole
    new_node->size = no_pages * PAGE_SIZE;
    new_node->end_virtual_address = new_node->start_virtual_address + new_node->size - 1;
    new_node->memory = mmap(NULL, no_pages * PAGE_SIZE, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

    return new_node;
}

main_node *add_main_node(int no_pages)
{
    // check if free_list_allocation is enough to allocate a new main_node
    if (sizeof(main_node) > remaining_free_list_allocation)
    {
        free_list_allocation = mmap(NULL, PAGE_SIZE, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
        current_free_list_allocation = free_list_allocation;
        remaining_free_list_allocation = PAGE_SIZE;
    }

    // allocate a new main_node
    main_node *new_node = (main_node *)current_free_list_allocation;
    current_free_list_allocation += sizeof(main_node);
    remaining_free_list_allocation -= sizeof(main_node);

    new_node->next = NULL;
    new_node->pages = no_pages;
    new_node->sub_head = add_sub_node(no_pages, new_node);
    new_node->sub_tail = new_node->sub_head;
    new_node->start_virtual_address = new_node->sub_head->start_virtual_address;
    new_node->end_virtual_address = new_node->sub_head->end_virtual_address;

    if (main_head == NULL)
    {
        new_node->prev = NULL;
        // adding to free list
        main_head = new_node;
        main_tail = new_node;
    }
    else
    {
        new_node->prev = main_tail;
        // adding to free list
        main_tail->next = new_node;
        main_tail = new_node;
    }

    return new_node;
}

void *split_sub_node(sub_node *sub_node_for_split, sub_node *sub_tail, int size)
{
    if (sizeof(sub_node) > remaining_free_list_allocation)
    {
        free_list_allocation = mmap(NULL, PAGE_SIZE, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
        current_free_list_allocation = free_list_allocation;
        remaining_free_list_allocation = PAGE_SIZE;
    }

    sub_node *new_node = (sub_node *)current_free_list_allocation;
    current_free_list_allocation += sizeof(sub_node);
    remaining_free_list_allocation -= sizeof(sub_node);

    // matching the end virtual address
    new_node->end_virtual_address = sub_node_for_split->end_virtual_address;

    // updating the start virtual address of new node
    sub_node_for_split->end_virtual_address = sub_node_for_split->start_virtual_address + size - 1;
    new_node->start_virtual_address = sub_node_for_split->end_virtual_address + 1;

    // printf("new_node->start_virtual_address = %d\n", new_node->start_virtual_address);
    new_node->memory = sub_node_for_split->memory + size;

    // updating the pointers for prev and next (if new node in middle for 2 nodes)
    if (sub_node_for_split->next != NULL)
        sub_node_for_split->next->prev = new_node;

    new_node->next = sub_node_for_split->next;
    sub_node_for_split->next = new_node;
    new_node->prev = sub_node_for_split;

    // updating sub_tail
    if (sub_tail == sub_node_for_split)
        sub_tail = new_node;

    new_node->type = 0;           // Hole
    sub_node_for_split->type = 1; // Process
    new_node->size = sub_node_for_split->size - size;
    sub_node_for_split->size = size;

    return sub_node_for_split->start_virtual_address;
}
/*
Allocates memory of the specified size by reusing a segment from the free list if
a sufficiently large segment is available.

Else, uses the mmap system call to allocate more memory on the heap and updates
the free list accordingly.

Note that while mapping using mmap do not forget to reuse the unused space from mapping
by adding it to the free list.
Parameter: The size of the memory the user program wants
Returns: MeMS Virtual address (that is created by MeMS)
*/
void *mems_malloc(size_t size)
{
    int no_pages = (int)(size / (PAGE_SIZE + 1)) + 1;
    if (main_head == NULL)
    {
        main_node *new_node = add_main_node(no_pages);
        if (new_node->sub_head->size > size)
        {
            return split_sub_node(new_node->sub_head, new_node->sub_tail, size);
        }
        else
        {
            new_node->sub_head->type = 1;
            return new_node->sub_head->start_virtual_address;
        }
    }

    // loop through the free list and find the first hole that can fit the required size
    main_node *main_loop_pointer = main_head;
    sub_node *sub_loop_pointer = NULL;
    while (main_loop_pointer != NULL)
    {
        sub_loop_pointer = main_loop_pointer->sub_head;
        while (sub_loop_pointer != NULL)
        {

            if (sub_loop_pointer->type == 0 && sub_loop_pointer->size >= size)
            {
                // check if the hole is big enough to be split
                if (sub_loop_pointer->size > size)
                {
                    return split_sub_node(sub_loop_pointer, main_loop_pointer->sub_tail, size);
                }
                else
                {
                    sub_loop_pointer->type = 1;
                    return sub_loop_pointer->start_virtual_address;
                }
            }
            sub_loop_pointer = sub_loop_pointer->next;
        }
        main_loop_pointer = main_loop_pointer->next;
    }

    // if no hole is found then add a new main_node
    main_node *new_node = add_main_node(no_pages);
    if (new_node->sub_head->size > size)
    {
        return split_sub_node(new_node->sub_head, new_node->sub_tail, size);
    }
    else
    {
        new_node->sub_head->type = 1;
        return new_node->sub_head->start_virtual_address;
    }
}

/*
this function print the stats of the MeMS system like
1. How many pages are utilised by using the mems_malloc
2. how much memory is unused i.e. the memory that is in freelist and is not used.
3. It also prints details about each node in the main chain and each segment (PROCESS or HOLE) in the sub-chain.
Parameter: Nothing
Returns: Nothing but should print the necessary information on STDOUT
*/
void print_main_node(main_node *main_node)
{
    printf("MAIN[%ld:%ld]", (long int)main_node->start_virtual_address, (long int)main_node->end_virtual_address);
}
void print_sub_node(sub_node *sub_node)
{
    if (sub_node->type == 0)
        printf("H[%ld:%ld]", (long int)sub_node->start_virtual_address, (long int)sub_node->end_virtual_address);
    else
        printf("P[%ld:%ld]", (long int)sub_node->start_virtual_address, (long int)sub_node->end_virtual_address);
}

void mems_print_stats()
{
    printf("\n");

    int main_node_count = 0;
    int sub_node_count = 0;
    int total_pages = 0;
    int total_unused_memory = 0;
    int sub_chain[1000000];
    int sub_chain_count = 0;

    main_node *main_loop_pointer = main_head;
    sub_node *sub_loop_pointer = NULL;
    while (main_loop_pointer != NULL)
    {
        total_pages += main_loop_pointer->pages;
        print_main_node(main_loop_pointer);
        printf(" --> ");
        sub_loop_pointer = main_loop_pointer->sub_head;
        sub_chain_count = 0;
        while (sub_loop_pointer != NULL)
        {
            if (sub_loop_pointer->type == 0)
                total_unused_memory += sub_loop_pointer->size;

            print_sub_node(sub_loop_pointer);
            printf(" <--> ");
            sub_loop_pointer = sub_loop_pointer->next;
            sub_node_count++;
            sub_chain_count++;
        }
        printf("NULL\n");
        main_loop_pointer = main_loop_pointer->next;
        sub_chain[main_node_count] = sub_chain_count;
        main_node_count++;
    }
    printf("\n");
    printf("Total Pages: %d\n", total_pages);
    printf("Total Unused Memory: %d\n", total_unused_memory);
    printf("Total Main Nodes: %d\n", main_node_count);
    printf("Total Sub Nodes: %d\n", sub_node_count);
    printf("Sub Chain: [");
    for (int i = 0; i < main_node_count; i++)
    {
        printf("%d, ", sub_chain[i]);
    }
    printf("]\n");
    printf("\n");
}

/*
Returns the MeMS physical address mapped to ptr ( ptr is MeMS virtual address).
Parameter: MeMS Virtual address (that is created by MeMS)
Returns: MeMS physical address mapped to the passed ptr (MeMS virtual address).
*/
void *mems_get(void *v_ptr)
{
    main_node *main_loop_pointer = main_head;
    sub_node *sub_loop_pointer = NULL;
    while (main_loop_pointer != NULL)
    {
        sub_loop_pointer = main_loop_pointer->sub_head;
        while (sub_loop_pointer != NULL)
        {
            if (v_ptr >= sub_loop_pointer->start_virtual_address && v_ptr <= sub_loop_pointer->end_virtual_address)
            {
                return sub_loop_pointer->memory + (v_ptr - sub_loop_pointer->start_virtual_address);
            }
            sub_loop_pointer = sub_loop_pointer->next;
        }
        main_loop_pointer = main_loop_pointer->next;
    }
    return NULL;
}

/*
this function free up the memory pointed by our virtual_address and add it to the free list
Parameter: MeMS Virtual address (that is created by MeMS)
Returns: nothing
*/
void mems_free(void *v_ptr)
{
    main_node *main_loop_pointer = main_head;
    sub_node *sub_loop_pointer = NULL;
    sub_node *freed_sub_node = NULL;
    int check_break = 0;
    while (main_loop_pointer != NULL && check_break == 0)
    {
        sub_loop_pointer = main_loop_pointer->sub_head;
        while (sub_loop_pointer != NULL)
        {
            if (v_ptr >= sub_loop_pointer->start_virtual_address && v_ptr <= sub_loop_pointer->end_virtual_address && sub_loop_pointer->type == 1) // TODO:removing the type check sub_loop_pointer->type == 1
            {
                sub_loop_pointer->type = 0;
                freed_sub_node = sub_loop_pointer;
                check_break = 1;
                break;
            }
            sub_loop_pointer = sub_loop_pointer->next;
        }
        main_loop_pointer = main_loop_pointer->next;
    }

    if (freed_sub_node == NULL)
        return;

    int prev_type = 0;
    int next_type = 0;
    if (freed_sub_node->prev == NULL)
        prev_type = 1;
    else
        prev_type = freed_sub_node->prev->type;

    if (freed_sub_node->next == NULL)
        next_type = 1;
    else
        next_type = freed_sub_node->next->type;

    if (prev_type == 1 && next_type == 1)
        return;

    // check if the freed sub_node is adjacent to any other freed sub_node
    sub_node *left_node = NULL;
    sub_node *right_node = NULL;
    if (prev_type == 0)
    {
        right_node = freed_sub_node;
        left_node = right_node->prev;

        left_node->end_virtual_address = right_node->end_virtual_address;
        left_node->size += right_node->size;
        left_node->next = right_node->next;
        if (right_node->next != NULL)
            right_node->next->prev = left_node;

        munmap(right_node, sizeof(right_node));
    }
    if (next_type == 0)
    {
        if (left_node == NULL)
            left_node = freed_sub_node;

        right_node = left_node->next;

        left_node->end_virtual_address = right_node->end_virtual_address;
        left_node->size += right_node->size;
        left_node->next = right_node->next;
        if (right_node->next != NULL)
            right_node->next->prev = left_node;

        munmap(right_node, sizeof(right_node));
    }
    return;
}