#define _GNU_SOURCE
#include <stdint.h>
#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdlib.h>

typedef struct node {
    uint64_t next;
    uint8_t u_data[504];
} node;

bool contains(int fd, uint64_t val) {
    uint64_t num;
    if(lseek(fd, 0, SEEK_SET) < 0) { err(9, "cannot lseek"); }
    ssize_t read_bytes = 0;
    while ((read_bytes = read(fd, &num, sizeof(num))) == sizeof(num)) {
        if(num == val) {
            return true;
        }
    }

    if(read_bytes < 0) { err(10, "cannot read"); }
    return false;
}

int main(int argc, char* argv[]) {
    if (argc != 2) { errx(1, "incorrect usage"); }
    //printf("size of struct: %ld\n", sizeof(node));
    int fd = open(argv[1], O_RDWR);
    if (fd < 0) { err(2, "cannot open file"); }
    struct stat f;
    if(fstat(fd, &f) < 0) { err(3, "cannot stat file"); }
    if(f.st_size % sizeof(node) != 0) { errx(4, "incorrect input file format"); }
    
    ssize_t read_bytes = 0;
    node c_node;
    uint64_t curr = 0;
    char tmp[] = "XXXXXX";
    int tmp_fd = mkstemp(tmp);
    if (tmp_fd < 0) { err(5, "cannot create tmp_file"); }
    size_t records_count = f.st_size / sizeof(node);
    while((read_bytes = read(fd, &c_node, sizeof(c_node))) > 0) {
        //printf("next: %ld\n", c_node.next);
        if (write(tmp_fd, &curr, sizeof(curr)) != sizeof(curr)) { err(6, "cannot write"); }
        if (c_node.next == 0 || c_node.next >= records_count) {
            break;
        }
        if(lseek(fd, c_node.next * sizeof(c_node), SEEK_SET) < 0) { err(7, "cannot lseek"); }
        curr = c_node.next;
    }

    if(read_bytes < 0) { err(8, "cannot read"); }

    for (uint64_t i = 1; i < records_count; i++) {
        if(!contains(tmp_fd, i)) {
            printf("will zero out %ld entry\n", i);
            // write 512B nulls starting from byte with num i * sizeof(node)
            if(lseek(fd, i * sizeof(node), SEEK_SET) < 0) { err(11, "cannot lseek"); }
            uint8_t zeros[512] = {0};
            if(write(fd, &zeros, sizeof(zeros)) != sizeof(zeros)) { err(12, "cannot write"); }
        }
    }

    close(fd);
    close(tmp_fd);
    unlink(tmp);
    exit(0);
}
