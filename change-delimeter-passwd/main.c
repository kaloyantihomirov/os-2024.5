#include <stdio.h>
#include <err.h>
#include <sys/fcntl.h>
#include <unistd.h>
#include <stdlib.h>

ssize_t lseekHandle(int fd, ssize_t offset, int whence) {
    ssize_t pos = lseek(fd, offset, whence);

    if(pos < 0) {
        close(fd);
        err(1, "Cannot reposition read/write offset.");
    }

    return pos;
}

int main(int argc, char ** argv)
{
    if (argc != 4) {
        errx(1, "Wrong input! Expected file name "
                "and two char delimiters: replace the first one by the second.");
    }

    const char* filename = argv[1];
    const char* replaceWhat = argv[2];
    const char* replaceWith = argv[3];

    int fd = open("passwd", O_RDWR);
    if(fd < 0) {
        err(1, "Could not open file %s.", filename);
    }

    char c;
    ssize_t read_bytes;
    while((read_bytes = read(fd, &c, sizeof(c))) == sizeof(c)) {
        if(c == *replaceWhat) {
            lseekHandle(fd, -1, SEEK_CUR);

            if (write(fd, replaceWith, 1) != 1) {
                close(fd);
                err(1, "Could not write to %s.", filename);
            }

        }
    }

    close(fd);

    if(read_bytes < 0) {
        err(1, "Could not read from %s.", filename);
    }

    exit(0);
}
