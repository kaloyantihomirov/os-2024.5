#include <err.h>
#include <sys/fcntl.h>
#include <unistd.h>
#include <stdlib.h>

int main(int argc, char ** argv)
{
    if(argc != 2) {
        errx(1, "Wrong command input! Expected file name.");
    }

    char* filename = argv[1];

    int fd = open(filename, O_RDONLY);
    if(fd < 0) {
        err(1, "Cannot open file %s", filename);
    }

    char c;
    ssize_t read_bytes;
    size_t newLinesMetSoFar = 0;
    while ((read_bytes = read(fd, &c, sizeof(c))) > 0) {
        ssize_t written_bytes = write(STDOUT_FILENO, &c, sizeof(c));

        if(written_bytes < 0 || read_bytes != written_bytes) {
            close(fd);
        }

        if(written_bytes < 0) {
            err(1, "Cannot write to standard output");
        }

        if(read_bytes != written_bytes) {
            err(1, "Cannot write all bytes to standard output");
        }

        if (c == '\n') {
            newLinesMetSoFar++;
        }

        if(newLinesMetSoFar >= 10) {
            break;
        }
    }

    if(read_bytes < 0) {
        close(fd);
        err(1, "Cannot read from file %s", filename);
    }

    close(fd);
    exit(0);
}
