#include <stdio.h>
#include <err.h>
#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>

typedef struct ReadBlock {
    uint32_t numIndex;
    uint32_t len;
} ReadBlock;

int main(int argc, const char** argv)
{
    if (argc != 4) {
        errx(1, "wrong input, expected 3 arguments");
    }

    const char* f1 = argv[1];
    const char* f2 = argv[2];
    const char* f3 = argv[3];
    
    int fd1 = open(f1, O_RDONLY);
    if(fd1 < 0) {
        err(1, "could not open file %s", f1);
    }

    int fd2 = open(f2, O_RDONLY);
    if(fd2 < 0) {
        err(1, "could not open file %s", f2);
    }

    int fd3 = open(f3, O_CREAT | O_TRUNC, 644);
    if(fd3 < 0) {
        err(1, "could not truncate or create file %s", f3);
    }

    int statResult;
    struct stat st1;
    statResult = fstat(fd1, &st1);
    if(statResult < 0) {
        err(1, "could not stat %s", f1);
    }
    struct stat st2;
    statResult = fstat(fd2, &st2);
    if(statResult < 0) {
        err(1, "could not stat %s", f2);
    }

    if(st1.st_size % sizeof(ReadBlock) != 0) {
        errx(1, "invalid format of %s", f1);
    }

    if(st2.st_size % sizeof(uint32_t) != 0) {
        errx(1, "invalid format of %s", f2);
    }

    ReadBlock readBlock;
    ssize_t readBytes;
    uint32_t* nums;

    while((readBytes = read(fd1, &readBlock, sizeof(readBlock)) == sizeof(readBlock))) {
        if(readBlock.len == 0) {
            continue;
        }

        size_t lastNumToReadStartByte =
                readBlock.numIndex * sizeof(uint32_t)
                + (readBlock.len - 1) * sizeof(uint32_t);

        // Note that it is enough to check for the start byte,
        // as we're sure the bytes in f2 are % by 4
        // By we're sure, I mean I have checked this already on line 56
        if(lastNumToReadStartByte > st2.st_size) {
            errx(1, "invalid tuple");
        }

        off_t offset = lseek(fd2, readBlock.numIndex * sizeof(uint32_t), SEEK_SET);

        if(offset < 0) {
            err(1, "could not move pointer in %s", f2);
        }

        nums = (uint32_t*)malloc(readBlock.len);
        if(nums == NULL) {
            err(1, "could not allocate mem");
        }

        size_t numsSize = readBlock.len * sizeof(nums[0]);

        ssize_t readBytesFromF2 = read(fd2, &nums, numsSize);
        if(readBytesFromF2 < 0) {
            err(1, "could not read from file %s", f2);
        }

        if(readBytesFromF2 < numsSize) {
            err(1, "could not read all bytes from %s", f2);
        }

        ssize_t writtenBytes = write(fd3, nums, numsSize);
        if(writtenBytes < 0) {
            err(1, "could not write to %s", f3);
        }

        if(writtenBytes < numsSize) {
            err(1, "could not write all bytes to %s", f3);
        }
    }

    if(readBytes != 0) {
        err(1, "could not read from %s", f1);
    }

    close(fd1);
    close(fd2);
    close(fd3);
    free(nums);

    return 0;
}
