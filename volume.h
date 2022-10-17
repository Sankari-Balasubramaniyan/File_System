#ifndef _VOLUME_H_
#define _VOLUME_H_

#include "disk.h"


#define SUPERMAGIC 0xCAFEBABE


struct super_block {
    unsigned int s_magic;                   // specified constant number for checking if any error produced
    int s_serial;                 //unique identifier for the volume (#track, #sector)
    char s_name[32];              // text name
    int s_inode;               // index of first file on the partition(volume)
    int s_free_block;            // index of the first position of free block
};

struct free_block {
    int f_nb;                   // amount of block in a free block chunck
    int next;                   //Next free block chunck number
};

struct super_block sb;

void init_volume(unsigned int vol);
int load_super(unsigned int vol);
void save_super();
unsigned int new_block();
void free_block(unsigned int block);
int getFirstFB();
unsigned int getNumFree();
struct free_block load_free_block(unsigned int block_num);
void save_block(struct free_block block, int block_number);
#endif
