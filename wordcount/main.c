#include <stdio.h>
#include <stdlib.h>
#include <err.h>
#include <sys/fcntl.h>
#include <unistd.h>
#include <ctype.h>

ssize_t lseekHandle(int fd, int whence) {
    ssize_t pos = lseek(fd, 0, whence);

    if(pos < 0) {
        close(fd);
        err(1, "Cannot reposition read/write file offset");
    }

    return pos;
}

int main(int argc, char ** argv)
{
    if(argc != 2) {
        errx(1, "Wrong input format! Expected file name.");
    }

    char* filename = argv[1];

    int fd = open(filename, O_RDONLY);

    if(fd < 0) {
        err(1, "Cannot open file %s.", filename);
    }

    char c;
    char last_read = ' ';
    ssize_t read_bytes;
    size_t new_lines_count = 0;
    size_t words_count = 0;
    size_t bytes_count = lseekHandle(fd, SEEK_END);
    lseekHandle(fd, SEEK_SET);

    while((read_bytes = read(fd, &c, sizeof(c))) == sizeof(c)) {
        if(c == '\n') {
            new_lines_count++;
        }

        if(!isspace((unsigned char)c) && isspace(last_read)) {
            words_count++;
        }

        last_read = c;
    }

    close(fd);

    if(read_bytes < 0) {
        err(1, "Cannot read from %s.", filename);
    }

    printf("%8zu%8zu%8zu %s\n", new_lines_count, words_count, bytes_count, filename);
    exit(0);
}
