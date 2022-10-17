#include "disk.h"
#include "volume.h"
#include <assert.h>
#include <string.h>
#include "inode.h"

/*Test to validate our work

* calls the new_bloc() function until it returns an error;
* checks that the disk is full;
* iterate a random number of times on freeing a block free_bloc() ;
* show disk status (free size);
* allocates blocks as long as the disk is not full and returns the number of blocks that could be allocated.*/

int main(){

    printf("Start - initialise disk");
    init_disk();
    printf("Initialise the volume");
    init_volume(0);

    int i = new_block();
    while(i != -1)
    {
        printf("Calling new_block function\n");
        printf("Free block that is alloted is : %d\n", i);
        i = new_block();
    }
    
    printf("To check if the disk full - ");
    printf("Super block is pointing to which free blocks? %d\n", getFirstFB());
    
    printf("Free-ing block number : 2 \n");
    free_block(2);
    printf("Free-ing block number : 5 \n");
    free_block(5);
    printf("Free-ing block number : 7 \n");
    free_block(7);
    printf("Free-ing block number : 8 \n");
    free_block(8);
    
    printf("Free disk size : %d\n", getNumFree());
    
    
    /*while(getNumFree() != 0)
    {
        printf("Allocating block number : %d\n", getFirstFB());
        new_block();
        printf("Number of free space : %d\n", getNumFree());
    }*/
   
   	printf("Create an inode in block number : %d\n", create_inode(FILES));
    printf("Create an inode in block number : %d\n", create_inode(FILES));  
    printf("Delete inode number : 7 \n");
    delete_inode(7);
    printf("Free disk size : %d\n", getNumFree());
    printf("First free block : %d\n",getFirstFB());

}
