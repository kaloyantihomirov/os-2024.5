#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <err.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <stdbool.h>
#include <string.h>

int main(int argc, char* argv[]) {
    if (argc != 4) { errx(1, "incorrect usage"); }

    int fd = open(argv[1], O_RDONLY);
    if (fd < 0) { err(2, "can't read"); }
    int fd2 = open(argv[2], O_RDONLY);
    if (fd2 < 0) { err(3, "can't read"); }

    struct stat f2;
    if(fstat(fd2, &f2) < 0) { err(5, "can't stat"); }

    size_t left = 0;
    size_t right = f2.st_size - 1;
    uint32_t pos_of_null_byte_in_f1;

    // ./main.c dict ind a
    bool found = false;
    while (left <= right) {
        size_t m = left + (right - left) / 2;
        if(lseek(fd2, m, SEEK_SET) < 0) { err(6, "can't lseek"); }
        if(read(fd2, &pos_of_null_byte_in_f1, sizeof(pos_of_null_byte_in_f1)) != sizeof(pos_of_null_byte_in_f1)) { err(7, "can't read"); }
        if(lseek(fd, pos_of_null_byte_in_f1 + 1, SEEK_SET) < 0) { err(8, "can't lseek"); }
        char buff[63]; uint8_t read_bytes;
        read_bytes = read(fd, &buff, sizeof(buff));
        if (read_bytes <= 0) { err(9, "can't read"); }
        uint8_t word_size = 0;
        for (uint8_t i = 0; i < read_bytes && buff[i] != '\n'; i++) {
            word_size++;
        }
        char word[word_size + 1];
        word[word_size] = '\0';
        for(uint8_t i = 0; i < word_size; i++) {
            word[i] = buff[i];
        }
        if (strcmp(word, argv[3]) == 0) {
            found = true; char desc;
            if(lseek(fd, pos_of_null_byte_in_f1 + 2 + word_size, SEEK_SET) < 0) { err(13, "can't lseek"); }
            ssize_t read_desc_bytes;
            while((read_desc_bytes = read(fd, &desc, sizeof(desc))) > 0) {
                if(write(1, &desc, sizeof(desc)) < 0) {
                    err(11, "can't write to stdout");
                }
            }
            if(read_desc_bytes < 0) { err(10, "can't read"); } break;
        } else if (strcmp(word, argv[4]) < 0) {
            left = m + 1;
        } else {
            right = m - 1;
        }
    }

    if(!found) {
        char msg[] = "word doesn't exist :(:(:(";
        write(1, &msg, strlen(msg));
    }

    close(fd);
    close(fd2);
    exit(0);
}