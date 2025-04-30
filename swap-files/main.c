#include <stdlib.h>
#include <sys/fcntl.h>
#include <err.h>
#include <unistd.h>

char buffer[4096];
size_t read_bytes;
size_t written_bytes;

void readAndWriteTo(const char* from, const char* to, int oflagTo) {
    int fd1 = open(from, O_RDONLY);

    if(fd1 < 0) {
        err(1, "could not open file %s", from);
    }

    int fd2 = open(to, O_WRONLY | oflagTo, 0644);
    if(fd2 < 0) {
        close(fd1);
        err(1, "could not open file %s", to);
    }

    while ((read_bytes = read(fd1, buffer, sizeof(buffer))) > 0) {
        written_bytes = write(fd2, buffer, read_bytes);

        if(written_bytes < 0 || written_bytes != read_bytes) {
            close(fd1);
            close(fd2);
        }

        if(written_bytes < 0) {
            err(1, "could not write to file %s", to);
        }

        if(written_bytes != read_bytes) {
            err(1, "could not write all bytes to file %s", to);
        }
    }

    close(fd1);
    close(fd2);

    if(read_bytes < 0) {
        err(1, "could not read from %s", from);
    }
}

void readFromAndWriteTo(const char* fromName, int fromFd, const char* toName, int toFd) {
    //we assume the passed file descriptors are active & valid
    while ((read_bytes = read(fromFd, buffer, sizeof(buffer))) > 0) {
        written_bytes = write(toFd, buffer, read_bytes);

        if(written_bytes < 0 || written_bytes != read_bytes) {
            close(fromFd);
            close(toFd);
        }

        if(written_bytes < 0) {
            err(1, "could not write to file %s", toName);
        }

        if(written_bytes != read_bytes) {
            err(1, "could not write all bytes to file %s", toName);
        }
    }

    if(read_bytes < 0) {
        close(fromFd);
        close(toFd);
        err(1, "could not read from %s", fromName);
    }
}

int main(int argc, char** argv)
{
    if(argc != 3) {
        errx(1, "Wrong input! We expect two file names");
    }

    int fd1 = open(argv[1], O_RDWR);

    if(fd1 < 0) {
        err(1, "could not open file %s", argv[1]);
    }

    const char* tempFilename = "temp-30042025230415";
    int fd3 = open(tempFilename, O_RDWR | O_CREAT | O_TRUNC, 0644);
    if(fd3 < 0) {
        close(fd1);
        err(1, "could not open file %s", tempFilename);
    }

    readFromAndWriteTo(argv[1], fd1, tempFilename, fd3);
    size_t f1Size = lseek(fd1, 0, SEEK_CUR);

    int fd2 = open(argv[2], O_RDWR);
    if(fd2 < 0) {
        close(fd1);
        close(fd3);
        err(1, "could not open file %s", argv[2]);
    }

    lseek(fd1, 0, SEEK_SET);
    readFromAndWriteTo(argv[2], fd2, argv[1], fd1);
    size_t f2Size = lseek(fd2, 0, SEEK_CUR);
    if (ftruncate(fd1, f2Size) < 0) {
        err(1, "could not truncate file %s", argv[1]);
    }
    lseek(fd2, 0, SEEK_SET);
    lseek(fd3, 0, SEEK_SET);
    readFromAndWriteTo(tempFilename, fd3, argv[2], fd2);
    if ( ftruncate(fd2, f1Size) < 0) {
        err(1, "could not truncate file %s", argv[2]);
    }

    close(fd1);
    close(fd2);
    close(fd3);

//    readAndWriteTo(argv[1], tempFilename, O_CREAT | O_TRUNC);
//    readAndWriteTo(argv[2], argv[1], O_WRONLY);
//    readAndWriteTo(tempFilename, argv[2], O_WRONLY);

    exit(0);
}
