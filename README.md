# MeMS: Memory Management System [CSE231 OS Assignment 3]

## [Documentation](https://docs.google.com/document/d/1Gs9kC3187lLrinvK1SueTc8dHCJ0QP43eRlrCRlXiCY/edit?usp=sharing)

### How to run the exmaple.c

After implementing functions in mems.h follow the below steps to run example.c file

```
$ make
$ ./example
```

# MeMS (Memory Management System) Documentation

MeMS is a memory management system that provides a way to efficiently allocate and manage memory in a virtual address space. This documentation will explain the logic used in the provided code and detail the functions and their functionality.

## Table of Contents

## Table of Contents

1. Introduction
2. Function Implementations
3. mems_init()
4. mems_finish()
5. sub_node *add_sub_node(int no_pages, main_node *main_node)
6. main_node \*add_main_node(int no_pages)
7. void *split_sub_node(sub_node *sub_node_for_split, sub_node \*sub_tail, int size)
8. mems_malloc(size_t size)
9. mems_free(void\* ptr)
10. mems_print_stats()
11. mems_get(void\* v_ptr)
12. Usage
13. Error Handling
14. Edge Cases
15. Data Structures
16. Compiling and Running
17. GitHub Repository

## Introduction

MeMS is a memory management system that efficiently allocates and manages memory in a virtual address space. It maintains a list of available memory segments and can allocate memory blocks as needed. The MeMS system also keeps track of allocated and free memory segments.

The code provided implements MeMS with the following key functions:

## Function Implementations

### mems_init()

`void mems_init()`

This function initializes all the required parameters for the MeMS system. It sets up the initial state of the system, including the head of the free list, the starting MeMS virtual address, and other global variables.

### mems_finish()

`void mems_finish()`

The `mems_finish` function is called at the end of the MeMS system. Its primary purpose is to release any allocated memory using the `munmap` system call.

### add_sub_node(int no_pages, main_node \*main_node)

The `add_sub_node` function is responsible for adding a new `sub_node` to the MeMS system. A `sub_node` represents a segment of memory within a `main_node` in the MeMS. Let's break down the function step by step:

1. The function takes two parameters:

   - `no_pages`: An integer indicating the number of pages required for the new `sub_node`.
   - `main_node`: A pointer to the parent `main_node` to which the new `sub_node` will belong.

2. The function starts by checking whether the size of a `sub_node` structure is greater than the remaining free memory allocation (`remaining_free_list_allocation`). This check ensures that there is enough free memory in the allocation area to create a new `sub_node`.

   - If the available free memory is insufficient, the function uses the `mmap` system call to allocate a new page of memory (`PAGE_SIZE`) in the MeMS virtual address space. It also updates the `current_free_list_allocation` to point to this new page and resets the `remaining_free_list_allocation` to `PAGE_SIZE`. This ensures that there is enough space for a new `sub_node`.

3. The function then allocates a new `sub_node` by casting the memory pointed to by `current_free_list_allocation` to a `sub_node` pointer and incrementing `current_free_list_allocation` by the size of a `sub_node`. This effectively creates a new `sub_node` structure in the available memory.

4. It initializes various fields of the new `sub_node`:

   - `start_virtual_address`: Set to the current value of the global `virtual_addr`, which represents the starting virtual address of the `sub_node`.
   - `virtual_addr` is then incremented by the product of `no_pages` and `PAGE_SIZE` to update it to the next available virtual address.
   - `next` and `prev`: Set to `NULL` as the new `sub_node` is not part of a sub-chain yet.
   - `type`: Set to 0, indicating that this is a HOLE (unused memory segment).
   - `size`: Set to the product of `no_pages` and `PAGE_SIZE` to represent the total size of the `sub_node`.
   - `end_virtual_address`: Calculated by adding the `start_virtual_address` and `size - 1`, representing the last virtual address of the `sub_node`.
   - `memory`: Allocates memory for the data associated with the `sub_node`. It uses the `mmap` system call to allocate memory equal to `no_pages * PAGE_SIZE`. This memory is used to store the actual data associated with the `sub_node`.

5. Finally, the newly created `sub_node` is returned to the caller. This `sub_node` is ready to be used for memory allocation within a `main_node`.

In summary, the `add_sub_node` function is responsible for creating a new `sub_node` within a `main_node`. It manages the allocation of memory for the `sub_node` structure, assigns the necessary values to its fields, and ensures that there is enough free memory available for the new `sub_node`. This function plays a crucial role in managing memory segments within the MeMS system.

### main_node \*add_main_node(int no_pages)

`add_main_node` function:
The `add_main_node` function is responsible for creating and adding a new `main_node` to the MeMS system. Let's break down its functionality:

- It takes an integer parameter, `no_pages`, which indicates the number of pages the new `main_node` should have.

- The function starts by checking whether there is enough free memory in the allocation area to create a new `main_node`. It compares the size of a `main_node` structure with the remaining free memory allocation (`remaining_free_list_allocation`). If there is insufficient memory, it uses the `mmap` system call to allocate a new page of memory (`PAGE_SIZE`) in the MeMS virtual address space. It also updates the `current_free_list_allocation` to point to this new page and resets the `remaining_free_list_allocation` to `PAGE_SIZE`. This ensures that there is enough space for a new `main_node`.

- Next, the function allocates a new `main_node` by casting the memory pointed to by `current_free_list_allocation` to a `main_node` pointer and increments `current_free_list_allocation` by the size of a `main_node`. This effectively creates a new `main_node` structure in the available memory.

- It initializes various fields of the new `main_node`:

  - `next`: Set to `NULL` as the new `main_node` is not part of a main-chain yet.
  - `pages`: Set to the value of `no_pages`, indicating the number of pages that the `main_node` contains.
  - `sub_head` and `sub_tail`: Calls the `add_sub_node` function to create a new `sub_node` and sets both `sub_head` and `sub_tail` to this newly created `sub_node`. The `add_sub_node` function handles the allocation and initialization of the sub-node.
  - `start_virtual_address` and `end_virtual_address`: Set to the corresponding values of the `start_virtual_address` and `end_virtual_address` of the `sub_head` sub-node, representing the virtual address range managed by the `main_node`.

- It then checks whether the `main_head` is `NULL`, indicating that this is the first `main_node` to be created. If it is the first, the `prev` of the new `main_node` is set to `NULL`, and both `main_head` and `main_tail` are set to the new `main_node`. This adds the new `main_node` to the free list.

- If there are already existing `main_nodes`, the `prev` of the new `main_node` is set to the current `main_tail`, and the new `main_node` is linked to the end of the free list. The `main_tail` is updated to the new `main_node`.

- Finally, the new `main_node` is returned to the caller. This function allows for the creation of new `main_nodes` in the MeMS system.

### void *split_sub_node(sub_node *sub_node_for_split, sub_node \*sub_tail, int size)

`split_sub_node` function:
The `split_sub_node` function is responsible for splitting a `sub_node` into two separate `sub_nodes` when memory needs to be allocated. Let's break down its functionality:

- The function takes three parameters:

  - `sub_node_for_split`: A pointer to the `sub_node` that needs to be split.
  - `sub_tail`: A pointer to the current `sub_tail` of the `main_node` to which the `sub_node` belongs. This is used to update the `sub_tail` when a new `sub_node` is created.
  - `size`: An integer representing the size of the memory allocation to be split from the `sub_node`.

- Similar to the `add_sub_node` function, this function first checks whether there is enough free memory in the allocation area to create a new `sub_node`. If there isn't, it allocates a new page of memory (`PAGE_SIZE`) using the `mmap` system call, updates `current_free_list_allocation` to point to this new page, and resets `remaining_free_list_allocation` to `PAGE_SIZE`.

- It then allocates a new `sub_node` by casting the memory pointed to by `current_free_list_allocation` to a `sub_node` pointer and increments `current_free_list_allocation` by the size of a `sub_node`. This effectively creates a new `sub_node` structure in the available memory.

- The function proceeds to perform the split operation:

  - It matches the `end_virtual_address` of the new `sub_node` with that of the `sub_node_for_split`, ensuring that the split creates two adjacent segments.
  - It updates the `start_virtual_address` of the `sub_node_for_split` to reflect the allocated memory, effectively marking the portion of the `sub_node` that is used.
  - The `start_virtual_address` of the new `sub_node` is set to be the next available virtual address after the end of `sub_node_for_split`.
  - It updates the `memory` field of the new `sub_node` to point to the allocated memory in the MeMS virtual address space.

- The function also updates the pointers for the previous and next `sub_nodes` to

### mems_malloc(size_t size)

`void* mems_malloc(size_t size)`

The `mems_malloc` function is responsible for allocating memory in the MeMS (Memory Management System). It takes a `size` parameter, which represents the size of memory to be allocated, and returns a virtual address where this memory is allocated. The function follows a set of logic to efficiently manage memory allocation. Let's break down the function step by step:

1. Calculate the number of pages required to allocate the requested `size`. It divides `size` by the `PAGE_SIZE` (a system-defined constant) and adds 1. This ensures that you allocate at least enough memory to satisfy the requested size, even if it's not a multiple of the page size.

2. Check if the `main_head` (the main chain of memory segments) is `NULL`, which means that there are no existing memory segments. In this case, a new `main_node` is added using the `add_main_node` function with the calculated number of pages (`no_pages`).

   a. If the size of the `sub_head` (the sub-chain of segments within the main chain) of the newly added `main_node` is greater than or equal to the requested `size`, it attempts to split the `sub_head` to allocate the requested size by calling the `split_sub_node` function.

   b. If the `sub_head` is not split, it sets the `type` of the `sub_head` to 1 (indicating a PROCESS), and the `start_virtual_address` of the `sub_head` is returned as the allocated virtual address.

3. If there are existing memory segments in the `main_head`, the function loops through the main chain and then the sub-chain to find a suitable hole (a segment marked as a HOLE) that can accommodate the requested size.

   a. If a hole is found (`sub_loop_pointer->type` is 0) and the size of the hole is greater than or equal to the requested size, the function checks if the hole can be split to allocate the requested size. If it can be split, it calls `split_sub_node` to allocate the memory.

   b. If the hole cannot be split, it marks the hole as a PROCESS (`sub_loop_pointer->type` is set to 1), indicating that it is now in use, and returns the `start_virtual_address` of the hole as the allocated virtual address.

4. If no suitable hole is found in the existing memory segments, a new `main_node` is added, similar to the case when `main_head` is `NULL`. The new `main_node` is created with the calculated number of pages, and the allocation is attempted as described in step 2.

In summary, the `mems_malloc` function attempts to allocate memory in a way that optimizes the use of existing memory segments. It looks for holes in the memory chain and tries to split them if necessary to accommodate the requested size. If no suitable holes are found, it creates a new memory segment to fulfill the allocation request. The function returns a virtual address where the allocated memory is located.

### mems_free(void\* ptr)

The `mems_free` function is responsible for freeing memory in the MeMS (Memory Management System). It takes a virtual address (`v_ptr`) as a parameter and releases the memory associated with that virtual address. The function follows a set of steps to efficiently manage memory deallocation. Let's break down the function step by step:

1. The function starts by initializing several variables:

   - `main_loop_pointer`: It points to the current `main_node` in the main chain and is used to iterate through the main chain.
   - `sub_loop_pointer`: It points to the current `sub_node` in the sub-chain and is used to iterate through the sub-chain of the current `main_node`.
   - `freed_sub_node`: A pointer to a `sub_node` that represents the memory segment that is being freed. It is initialized to `NULL`.
   - `check_break`: A flag to indicate whether the freeing process has been completed. It is initialized to 0.

2. The function enters a nested loop structure to iterate through the main chain and the sub-chain of the MeMS system. The outer loop (`main_loop_pointer`) iterates through the main chain, while the inner loop (`sub_loop_pointer`) iterates through the sub-chain of the current `main_node`.

3. Within the inner loop, the function checks if the provided virtual address (`v_ptr`) falls within the address range of the current `sub_node`. It also checks whether the `sub_node` is marked as a PROCESS (type 1). If these conditions are met, it proceeds with freeing the memory.

   - If the virtual address (`v_ptr`) is within the address range and the `sub_node` is marked as a PROCESS, the function updates the `type` of the `sub_node` to 0 (HOLE) to indicate that the memory is now available for future allocations.

   - It also sets the `freed_sub_node` pointer to the current `sub_node` and sets the `check_break` flag to 1 to indicate that the freeing process is complete. The function then breaks out of the loops.

4. After the loops, the function checks whether a `freed_sub_node` has been found and successfully marked as a HOLE. If not, it means that the provided virtual address did not correspond to any allocated memory, so the function returns without further action.

5. The function then examines the adjacent `sub_nodes` to the freed `sub_node` to determine if they are also available for merging.

   - It checks the `type` of the previous (`prev_type`) and next (`next_type`) `sub_nodes`. If they are both marked as PROCESS (type 1), the function returns without further action because there is no adjacent free memory to merge.

6. If there is adjacent free memory, the function proceeds to merge the freed `sub_node` with adjacent free `sub_nodes`. It checks if the freed `sub_node` is adjacent to a free memory segment on the left or the right side.

   - If the `prev_type` is 0 (indicating that the previous `sub_node` is a HOLE), the function merges the freed `sub_node` with the previous `sub_node` on the left side. It adjusts the `end_virtual_address`, `size`, and pointers accordingly and calls `munmap` to release the memory occupied by the merged `sub_node`.

   - If the `next_type` is 0 (indicating that the next `sub_node` is a HOLE), the function merges the freed `sub_node` with the next `sub_node` on the right side. It adjusts the `end_virtual_address`, `size`, and pointers similarly and calls `munmap` to release the memory occupied by the merged `sub_node`.

7. After merging, the function returns, and the memory has been successfully freed and merged with adjacent free memory segments.

In summary, the `mems_free` function is responsible for releasing memory associated with a provided virtual address. It checks if the provided address corresponds to allocated memory, marks the memory as free, and, if adjacent free memory segments are found, merges them for efficient memory management.

### mems_print_stats()

`void mems_print_stats()`

This function prints statistics about the MeMS system, including the total number of mapped pages, the amount of unused memory, and details about each node in the main chain and each segment (PROCESS or HOLE) in the sub-chain.

### mems_get(void\* v_ptr)

`void* mems_get(void* v_ptr)`

`mems_get` returns the MeMS physical address mapped to the provided virtual address. It allows users to obtain the physical address corresponding to a virtual address.

## Usage

To use the MeMS system, you can follow these steps:

1. Call `mems_init()` to initialize the MeMS system.

2. Use `mems_malloc(size)` to allocate memory, where `size` is the required memory size. This function returns a virtual address.

3. If you need to free memory, call `mems_free(ptr)` where `ptr` is the virtual address previously allocated by `mems_malloc`.

4. To print MeMS system statistics, use `mems_print_stats()`.

5. Call `mems_finish()` to release allocated memory and finalize the MeMS system.

## Error Handling

The code includes error handling using functions like `perror` for handling errors during memory allocation using `mmap`. When `mmap` fails, it prints an error message and can be further customized for appropriate error handling.

## Edge Cases

The code handles edge cases such as joining two/three adjacent hole nodes to make a new hole node. This is a crucial part of efficient memory management.

## Data Structures

The MeMS system utilizes two main data structures:

- `main_node`: Represents a node in the main chain, which maintains information about the entire allocated memory range.
- `sub_node`: Represents a node in the sub-chain, containing details about a specific memory segment, whether it's a HOLE or PROCESS.

These data structures are used to manage and allocate memory effectively.
