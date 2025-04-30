#include <fcntl.h>
#include <stdlib.h>
#include <err.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

// read after the end of the file -> check what happens
int main(void) {

 char buffer[10];
 int fd = open("foo", O_RDWR);

 int read_bytes = read(fd,buffer, sizeof(buffer));

 int tmp = lseek(fd, 0, SEEK_CUR);

 write(fd, "de", 2);
 tmp = lseek(fd, 5, SEEK_END);
 printf("pos: %d", tmp);
 write(fd, "newText", 7);
 tmp = lseek(fd, 0, SEEK_CUR);
 printf("pos: %d", tmp);
 
 close(fd);
 exit(0);
}
