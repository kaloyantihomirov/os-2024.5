#include <stdint.h>
#include <fcntl.h>
#include <err.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

typedef struct {
    uint32_t id;
    uint32_t packets_count;
    uint64_t original_size;
} header;

int main(int argc, char* argv[]) {
    if (argc != 3) { errx(1, "incorrect usage"); }

    int fd = open(argv[1], O_RDONLY);
    if (fd < 0) { err(2, "can't open"); }
    
    header h;
    if(read(fd, &h, sizeof(h)) != sizeof(h)) { err(3, "can't read"); }
    if (h.id != 0x21494D46) { errx(4, "wrong file format"); }
    printf("packets_count=%d original_size=%ld\n", h.packets_count, h.original_size);
    int fd2 = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd2 < 0) { err(6, "can't open"); }
    for (uint32_t i = 0; i < h.packets_count; i++) {
        uint8_t packet_info;
        if(read(fd, &packet_info, sizeof(packet_info)) < 0) { err(5, "can't read"); }

        uint8_t n;
        if(packet_info >> 7 & 1) {
            n = (uint8_t)(packet_info & 127u);
            uint8_t byte_to_repeat;
            if (read(fd, &byte_to_repeat, sizeof(byte_to_repeat)) != sizeof(byte_to_repeat)) { err(10, "can't read"); }
            uint8_t buff[n + 1];
            for (uint8_t i = 0; i < n + 1; i++) { 
                buff[i] = byte_to_repeat;
            }
            if(write(fd2, buff, sizeof(buff)) != (ssize_t)sizeof(buff)) { err(11, "can't write"); } 

        } else {
            n = packet_info;
            uint8_t buff[255];
            ssize_t read_bytes;
            read_bytes = read(fd, buff, n + 1);
            if (read_bytes < 0) { err(7, "can't read"); }
            if (read_bytes != n + 1) { errx(8, "wrong file formaat"); }
            if (write(fd2, buff, n + 1) != n + 1) { err(9, "can't write"); }
        }
    }

    close(fd);
    close(fd2);
    exit(0);
}
