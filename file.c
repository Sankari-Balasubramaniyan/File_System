#include "inode.h"
#include "volume.h"
#include "disk.h"
#include "file_manipulation.h"
#include "file.h"
#include "volume.h"

#include <string.h>
#include <assert.h>
#include <stdio.h>

/* Return the next free entry*/
unsigned int new_entry(struct file_desc_t *dir_fd)
{
    /*Read entry_s structure using Read_ifile and check for a null block number*/
    struct entry_s entries;
    unsigned int block = 0;                                              /*Index of where the new entry could be added*/    
    seek2_ifile(dir_fd, 0);
    while(read_ifile(dir_fd, &entries, sizeof(struct entry_s))!=0)
    {
        if(entries.inode_num == 0) return block;                          /*There exist an unassigned null block*/
        block++;
    }
    
    return block;   /*No empty blocks available so a new entry has to be added at the directory at the returned index*/             
}

/*Find an entry in the directory */
int find_entry(struct file_desc_t *dir_fd, const char *basename)
{
   /*Read entry_s using Read_ifile and check for a block number that has the given base name
   Return its position*/
   
    struct entry_s entries;
    unsigned int block = 0;                                                    
    while(read_ifile(dir_fd, &entries, sizeof(struct entry_s))!=0)
    {
        if(!strcmp(entries.filename,basename)) return block;                          
        block++;
    }
    
    return 0;
}

/*Add an entry to the directory */
unsigned int add_entry(unsigned int dirnumber,unsigned int inumber, const char *basename)
{
    /*Check if the given file descriptor type is directory
      Find a new entry position
      Add the block number and corresponding basename to entry_s */
      
    struct entry_s entry;
    struct file_desc_t f_d, *fd = &f_d;
    int flag = 0; // failure
    entry.inode_num = inumber;
    strcpy(entry.filename, basename);
    open_ifile(fd,dirnumber);
    unsigned int block = new_entry(fd);
    seek2_ifile(fd, block*sizeof(struct entry_s));
    int num_bytes = write_ifile(fd, &entry, sizeof(struct entry_s));
    seek2_ifile(fd, 0);
    while(read_ifile(fd, &entry, sizeof(struct entry_s))!=0)
    {
        printf("File name %s\t, Inode number %u\n", entry.filename, entry.inode_num);
        
    }
    if(num_bytes == sizeof(struct entry_s)) flag = 1; // success
    close_ifile(fd);
    if(flag == 1) return 1; 
    return 0;
}

/*Delete an entry from the directory*/
unsigned int del_entry(unsigned int dirnumber, unsigned int inumber, const char *basename)
{
    /*Check if the given file descriptor type is directory
      Find a file using find_entry
      Remove them from the directory structure  (I.E. make them null) */
      
    struct entry_s entry;
    struct file_desc_t f_d, *fd = &f_d;
    int flag = 0;
    entry.inode_num = 0; // block num is 0 then it can be over written
    strcpy(entry.filename, "\0");
    
    open_ifile(fd,dirnumber);
    unsigned int block = find_entry(fd, basename);
    seek2_ifile(fd, block*sizeof(struct entry_s));
    
    if(write_ifile(fd, &entry, sizeof(struct entry_s)) == sizeof(struct entry_s)) flag = 1;
    close_ifile(fd);
    
    if(flag == 1) return 1;
    return 0;
}

/* Get block number from basename */
unsigned int inumber_of_basename(unsigned int idir, const char *basename)
{
    /*Check if idir is a directory
      Open file correspoding to directory number
      Find the basename position using find_entry 
      Move to that position and read the entry struct
      Return entry's block number */
    
    struct entry_s entry;
    struct file_desc_t f_d, *fd = &f_d;
    int flag = 0;
    
    open_ifile(fd,idir);
    unsigned int block = find_entry(fd, basename);
    seek2_ifile(fd, block*sizeof(struct entry_s));
    
    if(read_ifile(fd, &entry, sizeof(struct entry_s)) == sizeof(struct entry_s)) flag = 1;
    close_ifile(fd);
    
    if(flag == 1) return entry.inode_num;
    
    return 0;
}

/* Get block number from path */
unsigned int inumber_of_path(const char *pathname)
{
    /*Check if the pathname starts with '/'
      Iternate through basenames and find the last base name - if no other files or directories exist return super node inode number
      Get the inode corresponding to last basename
      Return it */
      
      if(*pathname != '/') return 0;
      
      char basename[10];
      unsigned int last_pos = strrchr(pathname, '/');              //Last occurence of '/' in pathname
      
      if(last_pos == 0) return sb.s_inode;
      int ini_pos = 0;
      unsigned int inumber = sb.s_inode;
      pathname++;
      while(*pathname)
      {
          char* pos = strchr(pathname, '/');
          if(pos == NULL) 
          {
              strcpy(basename, pathname);
              inumber = inumber_of_basename(inumber,basename);
              break;
          } 
          int length = pos - pathname;
          strncpy(basename, pathname, length);
          inumber = inumber_of_basename(inumber,basename);
          pathname += (length+1);
          ini_pos = pos;
      }
      
      return inumber;
}

unsigned int dinumber_of_path(const char *pathname, const char **basename)
{
    
    /* Check if the pathname starts with '/'
       Find the last position of '/' +1 and store it in basename
       Get the inode for directory of the basename and return it */
       
    if(*pathname != '/') return 0;
    unsigned int last_pos = strrchr(pathname, '/');              //Last occurence of '/' in pathname
    if(last_pos == 0) return sb.s_inode;
    *basename = pathname+last_pos+1;
    
    unsigned int dirnumber = inumber_of_path(pathname- (strlen(pathname) - last_pos));
    
    return dirnumber;
}

int create_file(const char *pathname, enum file_type_e type)
{
  /*Find block number from pathname(can be file or directory)
    call Create_ifile using type
    Add the blocknumber by calling add_entry */
    
    unsigned int dirnumber = inumber_of_path(pathname);
    printf("Directory block to which the new file/directory will be added : %u\n", dirnumber);
    unsigned int file_block = create_ifile(type);
    printf("New file/directory block number that is alloted : %u\n",file_block);
    
    char name[10];
    
    printf("Enter the Directory/file name you want to create : ");
    scanf("%s", name);
    
    unsigned int flag = add_entry(dirnumber, file_block, name);
    if(flag == 1) return 1;
    return 0;
}
  

int delete_file(const char *pathname)
{
    /*Find block number from pathname(can be file or directory)
      call delete_ifile using found blocknumber 
      Delete the blocknumber by calling del_entry*/
      
    unsigned int block = inumber_of_path(pathname);
    unsigned int inumber = 0;
    char basename[10];
    printf("Enter the file name to be deleted : ");
    scanf("%s", basename);
    if((inumber = inumber_of_basename(block,basename))==0) return 0;                 //File does not exist
    if(del_entry(block, inumber, basename) == 0) 
    {
        delete_ifile(inumber);
        return 0;
    }
    return 1;
}

int open_file(struct file_desc_t *fd, const char *pathname)
{
    unsigned int inumber = inumber_of_path(pathname);
    if(inumber == 0) return 0;
    
    return open_ifile(fd,inumber);
}

void close_file(struct file_desc_t *fd)
{
    close_ifile(fd);
}

void flush_file(struct file_desc_t *fd)
{
    flush_ifile(fd);
}

void seek_file(struct file_desc_t *fd, int offset)
{
    seek_ifile(fd,offset);
}

int readc_file(struct file_desc_t *fd)
{
    return readc_ifile(fd);
}

int writec_file(struct file_desc_t *fd, char c)
{
    return writec_ifile(fd,c);
}

int read_file(struct file_desc_t *fd, void *buf, unsigned int nbyte)
{
    return read_ifile(fd,buf,nbyte);
}

int write_file(struct file_desc_t *fd, const void *buf, unsigned int nbyte)
{
    return write_ifile(fd,buf,nbyte);
}






