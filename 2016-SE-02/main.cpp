#include <fcntl.h>
#include <stdlib.h>
#include <err.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

void print_number(int fd, uint16_t num) {
    char num_text[6];
    snprintf(num_text, sizeof(num_text), "%d", num);
    write(fd, num_text, strlen(num_text) + 1);
}

void num_test() {
    uint16_t num = 120;
    int fd = open("foo", O_RDWR | O_CREAT | O_TRUNC, 0644);
    print_number(fd, num);

    lseek(fd, 3, SEEK_SET);
    char c;
    read(fd, &c, 1);
    if (c == '\0') {
        printf("null byte?");
    }
    printf("read char: %c", c);
}


int main(int argc, char* argv[]) {
//    char c = 'K';
//    int fd = open("bar", O_WRONLY | O_CREAT, 0644);
//    write(fd, &c, 1);
//    write(1, &c, 1);
    char msg[] = "word doesn't exist :(:(:(\n";
    write(1, &msg, strlen(msg));
    printf("first argument size: %d\n", strlen(argv[1]));
}
