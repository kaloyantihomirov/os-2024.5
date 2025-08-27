// Basically the main thing that nudged me in the right direction
// was the question I asked myself "how are we supposed to use the fact that in each file the lines are sorted?".
// And then the 2 pointers problem came to my mind. Here we have n pointers and files instead of arrays, but the idea is the same.

#include <stdint.h>
#include <err.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>

typedef struct {
    uint64_t id;
    uint8_t text_len;
    uint8_t text[256]; // +1 for the terminating null
} element;

void fill_array(bool* arr, int size, bool val) {
    for (int i = 0; i < size; i++) {
        arr[i] = val;
    }
}

bool all_true(bool* arr, int size) {
    for (int i = 0; i < size; i++) {
        if (!arr[i]) {
            return false;
        }
    }

    return true;
}

int main(int argc, char* argv[]) {
    if (argc < 2 || argc > 21) { errx(1, "invalid usage"); }
    int fds[argc - 1];
    for (int i = 0; i < argc - 1; i++) {
        fds[i] = open(argv[i + 1], O_RDONLY);

        if (fds[i] < 0) { err(2, "can't open file %s, terminating...", argv[i+1]); }
    }
    
    element heads[argc - 1];
    for (int i = 0; i < argc - 1; i++) {
        if(read(fds[i], &heads[i].id, sizeof(heads[i].id)) < 0) { err(3, "can't read from file %s", argv[i+1]); }
        if(heads[i].id != 133742) { errx(4, "invalid format of file %s", argv[i+1]); }
        if(read(fds[i], &heads[i].text_len, sizeof(heads[i].text_len)) < 0) { err(3, "can't read from file %s", argv[i+1]); }
        ssize_t read_bytes;
        if((read_bytes = read(fds[i], heads[i].text, heads[i].text_len)) < 0) { err(4, "can't read from file %s", argv[i+1]); }
        heads[i].text[read_bytes] = '\0';
    }

    bool should_read_from[argc - 1];
    bool is_finished[argc - 1];
    fill_array(should_read_from, argc - 1, true);
    fill_array(is_finished, argc - 1, false);

    element lines[argc - 1];
    while (true) {
        for (int i = 0; i < argc - 1; i++) {
            if (!should_read_from[i]) continue;
            ssize_t read_bytes;
            if((read_bytes = read(fds[i], &lines[i].id, sizeof(lines[i].id))) < 0) { err(3, "can't read from file %s", argv[i+1]); }
            if (read_bytes == 0) { is_finished[i] = true; continue; }
            if(read(fds[i], &lines[i].text_len, sizeof(lines[i].text_len)) < 0) { err(3, "can't read from file %s", argv[i+1]); }
            if((read_bytes = read(fds[i], lines[i].text, lines[i].text_len)) < 0) { err(4, "can't read from file %s", argv[i+1]); }
            lines[i].text[read_bytes] = '\0';
        }

        /* Took me a while to debug and understand why we need this -> we want this index to always identify a file which we haven't finished reading;
           initialising it to 0 breaks (can break) this invariant
        */
	int min_idx = -1;
        for (int i = 0; i < argc - 1; i++) {
            if(is_finished[i]) { continue; }
            if (min_idx == -1) { min_idx = i; }
            else if (lines[i].id < lines[min_idx].id) { min_idx = i; }
        }

        if(all_true(is_finished, argc - 1)) {
            break;
        }

        char msg1[] = "име на роля ";
        char msg2[] = ": реплика ";
        write(1, msg1, strlen(msg1));
        write(1, heads[min_idx].text, strlen((char *)heads[min_idx].text));
        write(1, msg2, strlen(msg2));
        write(1, lines[min_idx].text, strlen((char *)lines[min_idx].text));
        char nl = '\n';
        write(1, &nl, sizeof(nl));
        fill_array(should_read_from, argc - 1, false);
        should_read_from[min_idx] = true;
    }

    exit(0);
}
