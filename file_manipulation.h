#ifndef _FILE_MANIPULATION_H_
#define _FILE_MANIPULATION_H_

#include "volume.h"
#include "inode.h"

struct file_desc_t
{
    unsigned int inode_num;
    unsigned int position;
    unsigned int size;
    unsigned char buffer[BLOCKSIZE];
    unsigned int buff_pos;
    int flag;  // is modified or not
};
 
unsigned int create_ifile(enum file_type_e type);
int delete_ifile(unsigned int inumber);

int open_ifile(struct file_desc_t *fd, unsigned int inumber);
void close_ifile(struct file_desc_t *fd);
void flush_ifile(struct file_desc_t *fd);
void seek_ifile(struct file_desc_t *fd, int r_offset); /* relative */
void seek2_ifile(struct file_desc_t *fd, int a_offset); /* absolute */

int readc_ifile(struct file_desc_t *fd);
int writec_ifile(struct file_desc_t *fd, char c);
int read_ifile(struct file_desc_t *fd, void *buf, unsigned int nbyte);
int write_ifile(struct file_desc_t *fd, const void *buf, unsigned int nbyte);
#endif
