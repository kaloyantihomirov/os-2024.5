#include <err.h>
#include <string.h>
#include <sys/fcntl.h>
#include <stdint.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

typedef bool (*cmp_fn_t)(uint16_t, uint16_t);

bool less_than(uint16_t a, uint16_t b) {
    return a < b;
}

bool greater_than(uint16_t a, uint16_t b) {
    return a > b;
}

cmp_fn_t get_comparator(const char* option) {
    if(strcmp(option, "max") == 0) {
        return greater_than;
    }

    if(strcmp(option, "min") == 0) {
        return less_than;
    }

    return NULL;
}

int main(int argc, char ** argv)
{
    dprintf(STDOUT_FILENO, "argv[0]: %s; argv[1]: %s; argv[2]: %s\n", argv[0], argv[1], argv[2]);

    const char wrongInputErrMsg[] = "Wrong input format.";

    if(argc != 3) {
        errx(1, "%s Expected 2 parameters (option / --min, --max, --print \\"
                                  " and filename).", wrongInputErrMsg);
    }

    if(strlen(argv[1]) < 2) {
        errx(1, "%s Invalid option length. Expected at least 2 symbols.", wrongInputErrMsg);
    }

    char* option = argv[1] + 2;
    dprintf(STDOUT_FILENO, "option: %s\n", option);

    if(strcmp(option, "min") != 0
        && strcmp(option, "max") != 0
        && strcmp(option, "print") != 0) {
        errx(1, "%s Invalid option content. Expected 'min', 'max' or 'print'.", wrongInputErrMsg);
    }

    const char* filename = argv[2];

    int fd = open(filename, O_RDONLY);
    if(fd < 0) {
        err(1, "Could not open file %s.", filename);
    }

    //If file's size is not divisible by 2, we can tell from here that on the last read operation
    //we would have a problem, but I don't think it makes sense to disregard the results found up
    //until that point in time.

    uint16_t num;
    ssize_t read_bytes;
    uint16_t globalExtrema;
    bool firstPass = true;

    while((read_bytes = read(fd, &num, sizeof(num))) == sizeof(num)) {
        if(strcmp(option, "print") == 0) {
            ssize_t writtenBytes = dprintf(STDOUT_FILENO, "%u\n", num);

            if(writtenBytes < 0) {
                close(fd);
                err(1, "Cannot write to standard output.");
            }
        }
        else {
            if(firstPass || (*get_comparator(option))(num, globalExtrema)) {
                globalExtrema = num;
                firstPass = false;
            }
        }
    }

    close(fd);

    if(read_bytes < 0) {
        err(1, "Cannot read from %s.", filename);
    }

    if(read_bytes != 0) /* = 1 */ {
        dprintf(STDOUT_FILENO, "current result: %u\n", globalExtrema);
        err(1, "Wrong file format. Expected a file, containing only 2-byte unsigned integers.");
    }

    if(strcmp(option, "print") == 0) {
        exit(0);
    }

    dprintf(STDOUT_FILENO, "result: %u\n", globalExtrema);
    exit(0);
}
