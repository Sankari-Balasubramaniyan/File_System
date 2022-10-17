#include "disk.h"
#include "volume.h"
#include "inode.h"
#include "file_manipulation.h"
#include "file.h"

#include <string.h>

// Create root directory
// Create home and etc directory inside root
// Create dir1, and dir2 inside home
// Create file1 and file2 inside dir1
// Create file3 and file4 inside dir2
// Load file1 (small file)
// Load file2 (Fill indirect block)
// Load file3 (small file)
// Load file4 (Fill double indirect block)
// Create blah file inside etc
// Load blah (small file)
// Print contents of a directory
// Print data of a file (file2)
// Create a new file in etc - load and read it
// Read last 256bytes of file4

/*Create a filesystem on current volume - create root directory*/
int create_fs()             
{
    
    unsigned int root_block = create_ifile(DIRECS);
    printf("inumber of root dir: %u\n", root_block);
    sb.s_inode = root_block;
    save_super();
    return 1;
}
/* Function to print the contents of a directory */
void print_direc(int dirnum, char* dirname)
{
     struct file_desc_t fdd, *fd = &fdd;
     open_ifile(fd, dirnum);
     struct entry_s entries;  
     seek2_ifile(fd,0);  
     printf("\n"); 
     printf("            Files/directories in directory - %s\n", dirname);                                      
     while(read_ifile(fd, &entries, sizeof(struct entry_s))!=0)
    {
        printf("                  %s\n",entries.filename);                  
    }
    
    close_ifile(fd);
}

/* Main function */
int main()
{
    printf("Start - initialise disk \n");
    init_disk();
    printf("Initialise the volume \n");
    init_volume(0);
    
    create_fs();
    
    printf("\n");
    if(create_file("/",DIRECS)==1) printf("******Home directory created successfully******\n");   //Input should be home
    printf("\n");
    if(create_file("/",DIRECS)==1) printf("******etc directory created successfully******\n");   //Input should be etc
    print_direc(1, "/");
    printf("\n");
    if(create_file("/home",DIRECS)==1) printf("******dir1 directory created successfully inside home directory******\n");  //Input should be dir1
    printf("\n");
    if(create_file("/home",DIRECS)==1) printf("******dir2 directory created successfully inside home directory******\n");  //Input should be dir2
    print_direc(2, "home");
    printf("\n");
    if(create_file("/home/dir1",FILES)==1) printf("******file1 created successfully inside dir1 directory******\n");  //Input should be file1
    printf("\n");
    if(create_file("/home/dir1",FILES)==1) printf("******file2 created successfully inside dir1 directory******\n");  //Input should be file2
    print_direc(5, "dir1");
    printf("\n");
    if(create_file("/home/dir2",FILES)==1) printf("******file3 created successfully inside dir2 directory******\n");  //Input should be file3
    printf("\n");
    if(create_file("/home/dir2",FILES)==1) printf("******file4 created successfully inside dir2 directory******\n");  //Input should be file4
    print_direc(7, "dir2");
    printf("\n");
    if(create_file("/etc",FILES) ==1) printf("******blah file created successfully inside etc directory******\n");;  //Input should be blah
    printf("\n");
    print_direc(4, "etc");
    
    struct file_desc_t f_d,*fd = &f_d;
    char buff1[100];
    char buff2[12000];
    char buff3[20000];
    
    /* Load the file 1*/
    printf("\n              ***LOADING FILE 1***\n");
    open_file(fd,"/home/dir1/file1");
    strcpy(buff1,"Hellooo, I am now able to write in file 1 of dir 1 directory and I am happieeee");
    write_file(fd, &buff1, 100);
    close_file(fd);
    
    
    /* Load the file 2*/
    printf("\n              ***LOADING FILE 2***\n");
    open_file(fd,"/home/dir1/file2");
    for(int i = 0; i<12000; i++)
    {
        buff2[i] = 'a';
    }
    write_file(fd, &buff2, 12000);
    close_file(fd);
    
    
    /* Load the file 3 */
    printf("\n              ***LOADING FILE 3***\n");
    open_file(fd,"/home/dir2/file3");
    strcpy(buff1,"I am now able to write in file 3 of dir 2 and this is just a small file so no more blah blah :P");
    close_file(fd);
    
    
    /* Load the file 4*/
   /* printf("\n              ***LOADING FILE 4***\n");
    open_file(fd,"/home/dir2/file4");
    for(int i = 0; i<27000; i++)
    {
        buff3[i] = 'c';
    }
    write_file(fd, &buff3, 27000);    //If I write more than 26000 bytes we reach double indirect
    close_file(fd);
    
    
    /* Print data of file1 */
    printf("\n            ***READING FROM FILE1***\n");
    open_file(fd,"/home/dir1/file1");
    char buf1[100];
    read_file(fd, buf1, 100);
    for(int i = 0; i<100; i++) printf("%c",buf1[i]);
    printf("\n");
    close_file(fd);
    
    /* Print data of file2 */
    printf("\n            ***READING FROM FILE2***\n");
    open_file(fd,"/home/dir1/file2");
    char buf[12000];
    read_file(fd, buf, 12000);
    for(int i = 0; i<12000; i++) printf("%c",buf[i]);
    printf("\n");
    close_file(fd);
    
    /* Print data of file3 */
    printf("\n            ***READING FROM FILE3***\n");
    open_file(fd,"/home/dir2/file3");
    read_file(fd, buf1, 100);
    for(int i = 0; i<100; i++) printf("%c",buf1[i]);
    printf("\n");
    close_file(fd);
    
    /* Read and print last 256 bytes of file4 */ 
    /*Does not work */
   /* printf("\n            ***READING FROM FILE4***\n");
    open_file(fd,"/home/dir2/file4");
    seek2_ifile(fd, fd->size - 1024);
    char buf3[256];
    read_file(fd, buf3, 256);
    for(int i = 0; i<256; i++) printf("%c",buf3[i]);
    close_file(fd); */
    
}


