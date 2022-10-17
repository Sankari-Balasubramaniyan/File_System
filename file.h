#ifndef _FILE_H_
#define _FILE_H_

#include "volume.h"
#include "file_manipulation.h"
#include "inode.h"

#define MAX_SIZE 270

struct entry_s
{
    char filename[10];
    unsigned int inode_num;
};

unsigned int new_entry(struct file_desc_t *dir_fd);
int find_entry(struct file_desc_t *dir_fd, const char *basename);
unsigned int add_entry();
unsigned int del_entry();
unsigned int inumber_of_basename(unsigned int idir, const char *basename);
unsigned int inumber_of_path(const char *pathname);
unsigned int dinumber_of_path(const char *pathname, const char **basename);
int create_file(const char *pathname, enum file_type_e type);
int delete_file(const char *pathname);
int open_file(struct file_desc_t *fd, const char *pathname);
void close_file(struct file_desc_t *fd);
void flush_file(struct file_desc_t *fd);
void seek_file(struct file_desc_t *fd, int offset);
int readc_file(struct file_desc_t *fd);
int writec_file(struct file_desc_t *fd, char c);
int read_file(struct file_desc_t *fd, void *buf, unsigned int nbyte);
int write_file(struct file_desc_t *fd, const void *buf, unsigned int nbyte);
#endif
