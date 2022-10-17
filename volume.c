#include "volume.h"
#include "disk.h"
#include <string.h>
#include <assert.h>

/* This code manages the super block and chains free block */

unsigned int n_freeblocks = NBBLOCKS-1;
int current_vol = 0;

//initialize the superblock of a volume
void init_volume(unsigned int vol)
{   
    sb.s_magic = SUPERMAGIC;
    sb.s_serial = vol;
    strcpy(sb.s_name,"init super_block");
    sb.s_free_block = 1;
    sb.s_inode = 0;
    save_super(0);
    
    struct free_block f;
    f.f_nb = NBBLOCKS-1;
    f.next = -1;
    save_block(f,1);
}    


/*read from buffer and load it to vol where the super block locates*/
int load_super(unsigned int vol)
{   
    unsigned char buffer[BLOCKSIZE];

    read_bloc(vol, 0, buffer);
    memcpy(&sb, buffer, sizeof(struct super_block));    
    assert(sb.s_magic == SUPERMAGIC);
    return 0;
}


/*copy content from super block to buffer, then write to current volume*/
void save_super()
{
    unsigned char buffer[BLOCKSIZE];
    memcpy(buffer, &sb, sizeof(struct super_block));
    write_bloc(current_vol, 0, buffer);
}


 /* Load a free block content and return it */
struct free_block load_free_block(unsigned int block_num)
{
    unsigned char buffer[BLOCKSIZE];
    
    struct free_block new_block;
    read_bloc(current_vol, block_num, buffer);
    memcpy(&new_block, buffer, sizeof(struct free_block));    
    assert(sb.s_magic == SUPERMAGIC);
    return new_block;
}


/* Save changes of the block in current volume */
void save_block(struct free_block block, int block_number)
{
    unsigned char buffer[BLOCKSIZE];
    memcpy(buffer, &block, sizeof(struct free_block));
    write_bloc(current_vol, block_number, buffer);
}


/* Allow the first free block to be alloted and change first free block*/
unsigned int new_block()
{  
    struct free_block f;
    int free = sb.s_free_block;
    if(n_freeblocks > 0)
    {
        f = load_free_block(sb.s_free_block);
        // here is equals to f_nf > 1, more than one free block in a chunk
        if(f.f_nb != 1)                                           
        {
            sb.s_free_block++;
            f.f_nb--;
            n_freeblocks--;
            save_block(f,sb.s_free_block);
        }
        
        else                                                       
        {
            sb.s_free_block = f.next;
            n_freeblocks --;
        }
        save_super();
        
        return free;
     }
     return -1;
}



/* If we do not have a pointer to the head of next set of free blocks 
and only a block number, we can not possibly insert the free block in 
ascending order of block numbers, so we insert the given block as the 
first free block*/
void free_block(unsigned int block)
{
    struct free_block new_free_block;
                                                                     
    new_free_block.f_nb = 1;
    new_free_block.next = sb.s_free_block;
    sb.s_free_block = block;
    n_freeblocks++;
    save_super();
    save_block(new_free_block, block);

}


/* Return head of free blocks*/
int getFirstFB()
{
    return sb.s_free_block;
}


/* Return number of free blocks in the current volume */
unsigned int getNumFree()
{
    return n_freeblocks;
}







