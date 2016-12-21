/* 
    File: my_allocator.h

    Author: R.Bettati
            Department of Computer Science
            Texas A&M University
    Date  : 08/02/08

    Modified:

*/

#ifndef _my_allocator_h_                   // include file only once
#define _my_allocator_h_

/*--------------------------------------------------------------------------*/
/* DEFINES */
/*--------------------------------------------------------------------------*/

typedef void *Addr;

/*--------------------------------------------------------------------------*/
/* INCLUDES */
/*--------------------------------------------------------------------------*/

#include <vector>

using namespace std;

/*--------------------------------------------------------------------------*/
/* DATA STRUCTURES */
/*--------------------------------------------------------------------------*/

#pragma pack(push)
#pragma pack(1)
struct Block {
    Block *next;
    int size;
    bool free;
};
#pragma pack(pop)

/*--------------------------------------------------------------------------*/
/* FORWARDS */
/*--------------------------------------------------------------------------*/



/*--------------------------------------------------------------------------*/
/* MODULE   MY_ALLOCATOR */
/*--------------------------------------------------------------------------*/



unsigned int init_allocator(unsigned int block_size,
                            unsigned int length);

/* This function initializes the memory allocator and makes a portion of
   ’length’ bytes available. The allocator uses a ’block_size’ as
   its minimal unit of allocation. The function returns the amount of 
   memory made available to the allocator. If an error occurred, 
   it returns 0. 
*/

int release_allocator();

/* This function returns any allocated memory to the operating system. 
   After this function is called, any allocation fails.
*/

Addr my_malloc(unsigned int length);

/* Allocate _length number of bytes of free memory and returns the
   address of the allocated portion. Returns 0 when out of memory. */

int my_free(Addr a);

/* Frees the section of physical memory previously allocated
   using ’my_malloc’. Returns 0 if everything ok. */

void print_addr(Addr a);

void print_block(Block *b);
void print_allocator();

#endif 
