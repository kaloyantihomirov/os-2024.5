#include <unistd.h>
#include <err.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <string.h>
#include <cerrno>
#include <cstdio>

const char* ding = "DING ";
const char* dong = "DONG\n";

int main(int argc, char* argv[]) {
   if (argc != 3) { errx(1, "incorrect usage"); }

    char* endptr;
    long n = strtol(argv[1], &endptr, 10);
    if (errno == ERANGE) { err(2, "can't strtol"); }
    if (*argv[1] == '\0' || *endptr != '\0') { errx(3, "expected a num"); }
    if (n <= 1) { errx(4, "invalid arg value: %s", argv[1]); }

    long d = strtol(argv[2], &endptr, 10);
    if (errno == ERANGE) { err(5, "can't strtol"); }
    if (*argv[2] == '\0' || *endptr != '\0') { errx(6, "expected a num"); }
    if (d <= 1) { errx(7, "invalid arg value: %s", argv[2]); }

    int p_c[2]; int c_p[2];
    if (pipe(p_c) < 0) { err(8, "can't pipe"); }
    if (pipe(c_p) < 0) { err(9, "can't pipe"); }
    int pid = fork();
    if (pid < 0) { err(10, "can't fork"); }
    
    if (pid) {
        close(p_c[0]);
        close(c_p[1]);
    } else {
        close(p_c[1]);
        close(c_p[0]);
    }

    for (int i = 0; i < n; i++) {
        if(pid) {
            write(1, ding, strlen(ding));
            if(write(p_c[1], "a", 1) < 0) { err(11, "can't write"); }
            ssize_t read_bytes; uint8_t _;
            while ((read_bytes = read(c_p[0], &_, 1)) == 0) { }
            if (read_bytes < 0) { err(14, "can't read"); }
            sleep(d);
        } else {
            ssize_t read_bytes; uint8_t _;
            while ((read_bytes = read(p_c[0], &_, 1)) == 0) { }
            if (read_bytes < 0) { err(12, "can't read"); }
            char buff[255];
//            snprintf(buff, sizeof(buff), "reached write for dong; read_bytes: %d", read_bytes);
            write(1, dong, strlen(dong));
            if(write(c_p[1], "a", 1) < 0) { err(13, "can't write"); }
        }
    }

    if (pid) {
        close(p_c[1]);
        wait(NULL);
    } else {
        close(c_p[1]);
    }

    exit(0);
}
