/* 
    File: my_allocator.c

    Author: <your name>
            Department of Computer Science
            Texas A&M University
    Date  : <date>

    Modified: 

    This file contains the implementation of the module "MY_ALLOCATOR".

*/

/*--------------------------------------------------------------------------*/
/* DEFINES */
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* INCLUDES */
/*--------------------------------------------------------------------------*/

#include<stdlib.h>
#include <math.h>
#include <iostream>
#include "my_allocator.h"

/*--------------------------------------------------------------------------*/
/* DATA STRUCTURES */
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* CONSTANTS */
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* FORWARDS */
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* FUNCTIONS FOR MODULE MY_ALLOCATOR */
/*--------------------------------------------------------------------------*/

/* Don't forget to implement "init_allocator" and "release_allocator"! */

Addr mp;
unsigned long long mp_long;
vector<Block *> vec;
int basic_block_size;
int max_block_size;

unsigned int init_allocator(unsigned int block_size, unsigned int length) {
    //this is currently using the following format for storing metadata
    //next (void pointer), size (int), free (bool), data
    //however, I am thinking we may be benefitted by having a previous
    //pointer instead of a size value...

    //make sure we account for the loss of memory in the my_alloc() function.
    mp = malloc(length);
    mp_long = (unsigned long long) mp;
    basic_block_size = block_size;
    //we will start with one block of max length, and it will be decreased as needed.
    int max_block_index = (int) ceil(log2(((double) length) / (double) basic_block_size));
    //vec[max_block_index-1] will contain the pointer to the largest possible block
    max_block_size = (int) pow(2, floor(log2((double) length)));

    vec.resize(max_block_index + 1, nullptr);

    Addr cp = mp; //used for inserting each block at appropriate place
    int curr_block_size = max_block_size;
    int curr_block_index = max_block_index;
    while (length > 0) {//iterates through all elements of vector, ending when there is
        // no memory left to initialize in the free pointer
        int floor_log = floor(log(length) / log(curr_block_size));
        length = length - floor_log * curr_block_size;
        if (floor_log > 0) {//skip the allocation if there is no block of curr_block_size
            vec[curr_block_index] = (Block *) cp;
            vec[curr_block_index]->next = nullptr; // currently no next block
            vec[curr_block_index]->size = curr_block_size; //this sets the size data
            vec[curr_block_index]->free = true;
            cp = (Addr) ((char *) cp + curr_block_size);
        }
        curr_block_size /= 2;
        curr_block_index--;

    }

    return 0;
}

int release_allocator() {
    free(mp);
    return 0;
}


extern Addr my_malloc(unsigned int length) {
    //this should use the left-most smallest available memory portion.
    int header_size = sizeof(Block *) + sizeof(int) + sizeof(bool);

    if (length <= 0 || length > max_block_size - header_size)
        return nullptr;

    //find smallest existing free block
    int i = (int) ceil(log2(((double) length + (double) header_size) / (double) basic_block_size));
    //int i = start_index - 1;
    if (i < 0) i = 0; //sometimes i is calculated below 0. if so, change it to 0.

    while (vec[i] == nullptr)
        ++i;
    //split the block until it is small enough

    Block *block = vec[i];
    int split_size = block->size / 2; // calculate the size of the half block
    int split_available_size = split_size - header_size; // calculate the available size of the half block
    while (split_available_size >= length && split_size >= basic_block_size) {

        Block *left_block = block; // get the address of the left half
        Block *right_block = (Block *) ((char *) block + split_size); // get the address of the right half
        // cast to a char* because otherwise we would be adding in units of sizeof(Block)

        vec[i] = block->next; // set the free list at size i to point to the next block of that size

        left_block->next = right_block; // set left_block's next pointer to right_block's address
        right_block->next = vec[i -
                                1]; // set right_block's next pointer to the address of the first block in the lower tier

        left_block->size = split_size; // set left_block's size to half of the original block
        right_block->size = split_size; // set right_block's size to half of the original block

        left_block->free = true; // set left_block's free flag to true
        right_block->free = true; // set right_block's free flag to true

        vec[i - 1] = left_block; // set the free list at the next lowest size to point to the new pair of blocks

        --i;

        // recalculate the values for the next iteration
        block = vec[i];
        split_size = block->size / 2; // calculate the size of the half block
        split_available_size = split_size - header_size; // calculate the available size of the half block
    }

    vec[i] = block->next; // remove the current block from the free list
    block->next = nullptr; // clear out the next pointer for security purposes
    block->free = false; // mark as used
    return block + 1; // accesses the memory space that is returned to the user
}

extern int my_free(Addr a) {
    // find block, mark it as free
    Block *block = (Block *) a - 1;
    block->free = true;

    // index of the current operating block size
    int index = (int) ceil(log2((block->size) / (double) basic_block_size));
    // add block to free list
    Block *next = vec[index];
    vec[index] = block;
    block->next = next;

    // find block's buddy
    Block *buddy = (Block *) ((((unsigned long long) block - mp_long) ^ block->size) + mp_long);

    while (buddy->free && buddy->size == block->size) {
        Block *first_block = min(block, buddy); // first block in sequence (which will retain header)
        Block *second_block = max(block, buddy); // second block in sequence (which will be overwritten)

        second_block->next = nullptr; // clear out the second block's next pointer for security purposes

        // find where the buddy is in the free list
        Block *curr = vec[index];
        while (curr->next != buddy) {
            curr = curr->next;
        }
        // remove the buddy from the free list
        curr->next = curr->next->next;

        // remove the block from the free list
        vec[index] = vec[index]->next; // make sure this works ok


        first_block->size *= 2; // update block size in the header

        // advance to the next tier up
        index++;
        // add block to the beginning of that free list
        next = vec[index];
        vec[index] = first_block;
        first_block->next = next;

        // set block and buddy to prepare for the next loop
        block = first_block;
        buddy = (Block *) ((((unsigned long long) block - mp_long) ^ block->size) + mp_long);
    }

    return 0;
}

//---------------- the following were used for debugging ---------------------/

void print_addr(Addr a) {
    if (a == nullptr)
        cout << "NULL";
    else {
        Block *b = (Block *) a - 1;
        print_block(b);
    }
}

void print_block(Block *b) {
    //prints address, size integer, free boolean
    cout << '[' << b << ':' << *(Addr *) b << ',' << *(int *) ((char *) b + 8) << ',' << *(bool *) ((char *) b + 12) <<
    "]";
}

void print_allocator() {
    for (int i = 0; i < vec.size(); ++i) {
        cout << '[' << i << ':' << vec[i] << ']';
        Block *curr_block = vec[i];
        while (curr_block != nullptr) {
            cout << "->";
            print_block(curr_block);
            curr_block = curr_block->next;
        }
        cout << "\n";
    }
}