#include <stddef.h>
#include <unistd.h>
#include <stdint.h>
#include <err.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <stdlib.h>

typedef struct {
	uint32_t magic;
	uint32_t count;
} data_header;

typedef struct {
	uint32_t magic1;
	uint16_t magic2;
	uint16_t reserved;
	uint64_t count;
} comp_header;

typedef struct {
	uint16_t type;
	uint16_t reserved[3];
	uint32_t offset1;
	uint32_t offset2;
} comp_element;

// everything is aligned, so we can read directly into the structures

off_t get_start_byte_num(uint32_t offset, size_t start_byte_num) {
	return start_byte_num + offset * sizeof(uint64_t);
}

void swap(int fd, size_t s1, size_t s2, uint64_t el1, uint64_t el2) {
	if(lseek(fd, s1, SEEK_SET) < 0) { err(21, "can't lseek"); }
	if (write(fd, &el2, sizeof(el2)) < 0) { err(22, "can't write"); }
	if(lseek(fd, s2, SEEK_SET) < 0) { err(23, "can't lseek"); }
	if (write(fd, &el1, sizeof(el1)) < 0) { err(24, "can't write"); }
}

int main(int argc, char* argv[]) {
	if (argc != 3) { errx(1, "incorrect usage"); }
	
	int fd1 = open(argv[1], O_RDWR);
	if (fd1 < 0) { err(2, "can't open"); }
	int fd2 = open(argv[2], O_RDONLY);
	if (fd2 < 0) { err(3, "can't open"); } 

	/* DATA */
	data_header dh;
	if(read(fd1, &dh, sizeof(dh)) < 0) { err(11, "can't read"); }
	if(dh.magic != 0x21796F4A) { errx(15, "invalid file %s format", argv[1]); }
	struct stat f1;
	if(fstat(fd1, &f1) < 0) { err(12, "can't stat"); }
	if ((uint64_t)f1.st_size != sizeof(data_header) + dh.count * sizeof(uint64_t)) { errx(13, "invalid file %s format", argv[1]); } 
	ssize_t first_byte_of_data_idx;
	if((first_byte_of_data_idx = lseek(fd1, 0, SEEK_CUR)) < 0) { err(15, "can't lseek"); }	

	/* COMPARATOR */	
	comp_header ch;
	if(read(fd2, &ch, sizeof(ch)) < 0) { err(4, "can't read"); }
	if(ch.magic1 != 0xAFBC7A37) { errx(5, "invalid format of file %s", argv[2]); }
	if(ch.magic2 != 0x1C27) { errx(6, "invalid format of file %s", argv[2]); }
	
	for (uint64_t i = 0; i < ch.count; i++) {
		comp_element ce;	
		if(read(fd2, &ce, sizeof(ce)) != sizeof(ce)) { err(9, "cant' read"); }
		if(ce.type != 0 && ce.type != 1) { errx(14, "invalid type in comparator element"); }
		size_t reserved_size = sizeof(ce.reserved) / sizeof(*ce.reserved);
		for (size_t j = 0; j < reserved_size; j++) { if (ce.reserved[j] != 0) { errx(10, "invalid format of file %s", argv[2]); }  }
		off_t s1 = get_start_byte_num(ce.offset1, first_byte_of_data_idx);
		off_t s2 = get_start_byte_num(ce.offset2, first_byte_of_data_idx);
		if (s1 >= f1.st_size || s2 >= f1.st_size) { errx(16, "referring to non-existing byte in %s", argv[1]); }
		uint64_t el1; uint64_t el2; 
		if(lseek(fd1, s1, SEEK_SET) < 0) { err(17, "can't lseek"); }
		if(read(fd1, &el1, sizeof(el1)) < 0) { err(18, "can't read"); }
		
		if(lseek(fd1, s2, SEEK_SET) < 0) { err(19, "can't lseek"); }
		if(read(fd1, &el2, sizeof(el2)) < 0) { err(20, "can't read"); }
		
		if(ce.type) {
			if (el1 < el2) { swap(fd1, s1, s2, el1, el2); }
		} else {
			if (el1 > el2) { swap(fd1, s1, s2, el1, el2); }	
		}
	}

	close(fd1);
	close(fd2);
	exit(0);
}
