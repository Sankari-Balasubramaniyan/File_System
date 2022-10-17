#include "inode.h"
#include "disk.h"
#include <string.h>
#include <assert.h>

unsigned int create_inode(enum file_type_e type)
{   
    struct inode new_inode;
    int inumber = new_block();
    
    if(inumber == -1){
        perror("No more free blocks available in current volume");
    }
    
    new_inode.index = inumber;
    new_inode.type = type;
    
    for(int i = 0; i < MAX_DIRECT_BLOCK; i++)
    {
        new_inode.direct_block[i] = 0;
    }
    
    new_inode.indirect_block = 0;
    new_inode.double_indirect_block = 0;
    new_inode.size = 0;
    
    write_inode(inumber, &new_inode);
    
    return new_inode.index;
}


/*to read the inode structure, therefore the inumber = block number*/
void read_inode(unsigned int inumber, struct inode *inode){
    
    unsigned char buffer[BLOCKSIZE];
    read_bloc(0, inumber, buffer);
    memcpy(inode, buffer, sizeof(struct inode));  

}


/*to modify then save the inode structure*/
void write_inode(unsigned int inumber, struct inode *inode)
{
    
    unsigned char buffer[BLOCKSIZE];
    memcpy(buffer, inode, sizeof(struct inode));
    write_bloc(0, inumber, buffer);
}

int delete_inode(unsigned int inumber)
{
    struct inode f_inode;
    
    unsigned int direct[MAX_INDIRECT_BLOCK];
    unsigned int indirect[MAX_DOUBLE_INDIRECT_BLOCK];
    
    
    read_inode(inumber,&f_inode);
    
    /* Free direct blocks */ 
    for(int i=0;i<MAX_DIRECT_BLOCK;i++)     
	{
        if(f_inode.direct_block[i] != 0)
            free_block(f_inode.direct_block[i]);
    }
    
    /* Free indirect blocks */
    if(f_inode.indirect_block != 0)
    {
        read_bloc(0, f_inode.indirect_block, (unsigned char *)&direct);                      //Go to indirect block number and get the direct block array
        
        for(int i=0;i<MAX_INDIRECT_BLOCK;i++) 
        {
            if(direct[i] != 0)
                free_block(direct[i]);
        }
        
        free_block(f_inode.indirect_block);
    }
    
    /* Free double indirect blocks */
    if(f_inode.double_indirect_block != 0) 
    {
        read_bloc(0, f_inode.double_indirect_block, (unsigned char *)&indirect);            //Go to double indirect block number and get the indirect block array
        
        for(int i=0;i<MAX_DOUBLE_INDIRECT_BLOCK;i++) 
        {
            if(indirect[i] != 0)
            {
                read_bloc(0, indirect[i], (unsigned char *)&direct); 
                
                for(int i=0;i<MAX_INDIRECT_BLOCK;i++)
                {
                    if(direct[i] != 0)
                        free_block(direct[i]);
                }
                
                free_block(indirect[i]);
            }
        }
        
        free_block(f_inode.double_indirect_block);
    }
    
    free_block(inumber);
    return 1;
}

/*return a particular inode block associate to fblock, if not allocated returns null*/
unsigned int vblock_of_fblock(unsigned int inumber, unsigned int fblock, int do_allocate)
{  
    struct inode f_inode;
    int newBlock;
    
    unsigned int direct[MAX_INDIRECT_BLOCK];
    unsigned int indirect[MAX_DOUBLE_INDIRECT_BLOCK];
   
    read_inode(inumber,&f_inode);
    
    /* If the given block is present in direct block */
    if(fblock < MAX_DIRECT_BLOCK) // fblock is not depassed direct block size
    {
        if(f_inode.direct_block[fblock] == 0) // is 0 so its free
        {
            if(do_allocate)  // depends on user if allocate or not
            {
                newBlock = new_block();
                if(newBlock != -1){
		f_inode.direct_block[fblock] = newBlock;
		write_inode(inumber,&f_inode);
		return newBlock;}
            }
            
            else return 0;
        }
        else return f_inode.direct_block[fblock]; // if already allocated then return the block number
    }
    
     /* If the given block is present in indirect block */
     fblock = fblock - MAX_DIRECT_BLOCK;
     if(fblock < MAX_INDIRECT_BLOCK)
     {
         if(f_inode.indirect_block == 0)
         { 
             if(do_allocate)
            {
                unsigned int directB[MAX_INDIRECT_BLOCK];
                newBlock = new_block();
                printf("Indirect block is alloted %u\n", newBlock);
                if(newBlock != -1)
                {
		    f_inode.indirect_block = newBlock;
		    for(int i = 0; i < MAX_INDIRECT_BLOCK; i++)
                    {
                        directB[i] = 0;
                    }
                    write_bloc(0, newBlock, directB);
                    write_inode(inumber,&f_inode);
		    return -2;}
            }
            
            else return 0;
         }
         else
         {
             
             read_bloc(0, f_inode.indirect_block, (unsigned char *)&direct);
             if(direct[fblock] == 0)
             {
                if(do_allocate)
                {
                    newBlock = new_block();
                    if(newBlock != -1){
		    direct[fblock] = newBlock;
		    write_bloc(0,f_inode.indirect_block, direct);
		    write_inode(inumber,&f_inode);
		    return newBlock;}
                }
            
                else return 0;
            }
            else return direct[fblock];
         }
      }
      
       /* If the given block is present in double indirect block */
       
      fblock = fblock + MAX_DIRECT_BLOCK;
      fblock = fblock - (MAX_DIRECT_BLOCK + MAX_INDIRECT_BLOCK);
      
      if(fblock < MAX_DOUBLE_INDIRECT_BLOCK)
      {
          if(f_inode.double_indirect_block == 0)
          {
              
              if(do_allocate)
              {
                  newBlock = new_block();
                  if(newBlock != -1)
                  {
                  printf("Double indirect block is alloted %u\n", newBlock);
                  unsigned int indirectB[MAX_DOUBLE_INDIRECT_BLOCK];
		  f_inode.double_indirect_block = newBlock;
		  for(int i = 0; i < MAX_DOUBLE_INDIRECT_BLOCK; i++)
                  {
                        indirectB[i] = 0;
                  }
                  write_bloc(0,f_inode.double_indirect_block, indirectB);
		  write_inode(inumber,&f_inode);
		  return -3;}
              }
              else return 0;
          }
          else
          {
              read_bloc(0, f_inode.double_indirect_block, (unsigned char *)&indirect);
              
              if(indirect[fblock/64] == 0)
             {
                if(do_allocate)
                {
                    newBlock = new_block();
                    if(newBlock != -1)
                    {
                    printf("Indirect block of double indirect block is alloted %u\n", newBlock);
                    unsigned int directB[MAX_INDIRECT_BLOCK];
		    indirect[fblock/64] = newBlock;
		    for(int i = 0; i < MAX_INDIRECT_BLOCK; i++)
                    {
                        directB[i] = 0;
                    }
                    write_bloc(0,newBlock, directB);
		    write_bloc(0,f_inode.double_indirect_block, indirect);
		    write_inode(inumber,&f_inode);
		    return -2;}
                }
            
                else return 0;
            }
            else
            {
               
                read_bloc(0, indirect[fblock/64], (unsigned char *)&direct);
                
                if(direct[fblock%64] == 0)
                {
                    if(do_allocate)
                    {
                    newBlock = new_block();
                    if(newBlock != -1){
		    direct[fblock%64] = newBlock;
		    write_bloc(0,indirect[fblock/64], direct);
		    write_bloc(0,f_inode.double_indirect_block, indirect);
		    write_inode(inumber,&f_inode);
		    return newBlock;}
                   }
                   else return 0;
                }
                else return direct[fblock%64];
            }
          }
      }
    
     return -1;
}



