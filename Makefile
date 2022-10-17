CC := gcc
CFLAGS := -Wall -Wextra -g


fileTest: disk.o volume.o inode.o file_manipulation.o file.o fileTest.o
	$(CC) $^ -o $@

testsb: disk.o volume.o testsb.o inode.o file_manipulation.o file.o 
	$(CC) $^ -o $@ 

exemple: disk.o volume.o exemple.o inode.o file_manipulation.o file.o 
	$(CC) $^ -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f *.o exemple testsb fileTest disk.img  

.PHONY: clean
