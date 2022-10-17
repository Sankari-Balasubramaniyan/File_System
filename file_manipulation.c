#include "inode.h"
#include "volume.h"
#include "disk.h"
#include "file_manipulation.h"

#include <string.h>
#include <assert.h>


unsigned int create_ifile(enum file_type_e type)
{
    return create_inode(type);
}
int delete_ifile(unsigned int inumber)
{
    return delete_inode(inumber);
}

/* Ideally stores the content of the first data block file in a temporary buffer */
int open_ifile(struct file_desc_t *fd, unsigned int inumber)               
{
    
    struct inode f_inode;
    
    read_inode(inumber, &f_inode);
    
    int first_data_block = 0;
    
    first_data_block = vblock_of_fblock(inumber, 0, 0);
    
    fd->inode_num = f_inode.index;
    fd->position = 0;
    fd->size = f_inode.size;                                                    
    
    fd->flag = 0;
    fd->buff_pos = (fd->position)%BLOCKSIZE;
    
    if(first_data_block == 0)                                           //Data block not allocated yet
    {
        memset(fd->buffer, 0, BLOCKSIZE);
    }
    else
    {
        read_bloc(0, first_data_block, fd->buffer);
    }
    
    return 1;
}

void close_ifile(struct file_desc_t *fd)
{
    struct inode f_inode;
    read_inode(fd->inode_num, &f_inode);
    f_inode.size = fd->size;
    write_inode(fd->inode_num, &f_inode);
    flush_ifile(fd);
}

void flush_ifile(struct file_desc_t *fd)
{
    if(fd->flag != 0)
    {
        unsigned int block =  fd->position/BLOCKSIZE;
        unsigned int bloc = vblock_of_fblock(fd->inode_num, block, 1);
        if(block == -2 || block == -3) bloc = vblock_of_fblock(fd->inode_num, block+1, 1);
        write_bloc(0, bloc, fd->buffer);
        
        fd->flag = 0;
    }
}

void seek_ifile(struct file_desc_t *fd, int r_offset) /* relative */
{
    seek2_ifile(fd, fd->position+r_offset);
}

void seek2_ifile(struct file_desc_t *fd, int a_offset) /* absolute */
{
    unsigned int cur_block_num = fd->position/BLOCKSIZE;
    
    unsigned int new_block_num = a_offset/BLOCKSIZE;
    unsigned int new_buff_pos =  a_offset%BLOCKSIZE;
    
    struct inode f_inode;
    read_inode(fd->inode_num, &f_inode);
    
    if(cur_block_num != new_block_num)                                     //Open a different block
    {
        flush_ifile(fd);
        int data_block_num = 0;
        data_block_num = vblock_of_fblock(fd->inode_num, new_block_num, 0);
        
        if(data_block_num == 0) //Reset cursor to 0th position no more blocks to read from
        {
            data_block_num = vblock_of_fblock(fd->inode_num, 0, 0);
        }
                                                   
        read_bloc(0, data_block_num, fd->buffer);
    }
    
    fd->position = a_offset;
    fd->buff_pos = new_buff_pos;
}

int readc_ifile(struct file_desc_t *fd)
{
    if(fd->position > fd->size || fd->size==0) return EOF;
    unsigned char var = fd->buffer[fd->buff_pos];
    seek_ifile(fd,1);
    return var;
}

int writec_ifile(struct file_desc_t *fd, char c)
{
    fd->buffer[fd->buff_pos] = c;
    
    /* If it is the first time writting and data block is not alloted, allot it and then write on it*/
    if(fd->flag == 0)
    {
        unsigned int data_block = vblock_of_fblock(fd->inode_num, (fd->position)/BLOCKSIZE, 1);
        if(data_block == -2) 
        {
            printf("************Reached indirect block °_° ************\n");
            fd->position += 256;
            data_block = vblock_of_fblock(fd->inode_num, ((fd->position)/BLOCKSIZE), 1);             //If the datablock is an indirect block, get the block num of first direct block of it 
        }
        
        
        if(data_block == -3)
        {
            printf("************Reached double indirect block °_° ************\n");
            fd->position += 256;
            data_block = vblock_of_fblock(fd->inode_num, ((fd->position)/BLOCKSIZE), 1);
            if(data_block == -2) 
            {
                printf("************Reached indirect block of double indirect block °_° ************\n");
                fd->position += 256;
                data_block = vblock_of_fblock(fd->inode_num, ((fd->position)/BLOCKSIZE), 1);             //If the datablock is an indirect block, get the block num of first direct block of it 
            }
        }
        if(data_block == -1) return -1; //No free block avaiable
        fd->flag = 1;                     
    }
    
    /* If the buffer is full, write it in the block and open the next block */
    if(fd->buff_pos == BLOCKSIZE-1)
    {
        struct inode f_inode;
        read_inode(fd->inode_num, &f_inode);
        flush_ifile(fd);
        unsigned int data_block = vblock_of_fblock(fd->inode_num, (fd->position+1)/BLOCKSIZE, 0);
        if(data_block ==  f_inode.indirect_block) 
        {
            data_block = vblock_of_fblock(fd->inode_num, ((fd->position+1)/BLOCKSIZE)+1, 0);             //If we have read an indirect block, open its first direct block
        }
        if(data_block ==  f_inode.double_indirect_block)
        {
            data_block = vblock_of_fblock(fd->inode_num, ((fd->position+1)/BLOCKSIZE)+2, 0);
        }
        if(data_block == 0) memset(fd->buffer, 0, BLOCKSIZE);
        read_bloc(0, data_block, fd->buffer);
    }
    
    //printf("Written in block number %u\n", vblock_of_fblock(fd->inode_num, (fd->position)/BLOCKSIZE, 0));  //Uncomment to follow which block the characters are written to
    /*Increment file size*/
    if (fd->size < fd->position) fd->size = fd->position;     
    
    /*Increment cursor*/           
    fd->position++;
    fd->buff_pos = (fd->position)%BLOCKSIZE;
    
    return fd->position-1;
}


int read_ifile(struct file_desc_t *fd, void *buf, unsigned int nbyte)
{
    for(unsigned int i = 0; i<nbyte; i++)
    {
        int var = readc_ifile(fd);
        if(var == EOF) return i;
        *((char *)buf + i) = var; 
    }   
    return nbyte;
}

int write_ifile(struct file_desc_t *fd, const void *buf, unsigned int nbyte)
{
    for(unsigned int i = 0; i<nbyte; i++)
    {
        if(writec_ifile(fd, *((char *)buf + i)) == -1) return -1;
    }
    
    return nbyte;
}






























