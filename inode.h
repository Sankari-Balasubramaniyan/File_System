#ifndef _INODE_H_
#define _INODE_H_

#include "volume.h"
#define MAX_DIRECT_BLOCK 40
#define MAX_INDIRECT_BLOCK 64
#define MAX_DOUBLE_INDIRECT_BLOCK 64                      //Size of double indirect block is 64 but can store about 64*64*256 file size due to 64 different lists of direct blocks

enum file_type_e {FILES, DIRECS};

struct inode {
    int index;
    int type;
    int direct_block[MAX_DIRECT_BLOCK];
    int indirect_block; 
    int double_indirect_block;
    int size;
};

int direct_block_2[MAX_INDIRECT_BLOCK];                 //Direct blocks in indirect_block
int in_direct_block[MAX_INDIRECT_BLOCK];                 //Indirect blocks in double_indirect_block
int direct_block_3[MAX_INDIRECT_BLOCK];                 //Direct block in in_direct_block

void read_inode(unsigned int inumber, struct inode *inode);
void write_inode(unsigned int inumber, struct inode *inode);
unsigned int create_inode(enum file_type_e type);
int delete_inode(unsigned int inumber);
//unsigned int vblock_of_fblock(unsigned int inumber, unsigned int fblock);
unsigned int vblock_of_fblock(unsigned int inumber, unsigned int fblock, int do_allocate);
unsigned int inode_size(struct inode *inode);
#endif
