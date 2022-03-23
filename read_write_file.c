#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h> 
#include <sys/types.h> 
#include <fcntl.h> 
#include <sys/ioctl.h>

#define FILE_NAME   "/dev/drvo_device"
#define MAX_SIZE    255

int     readWriteFile = 0;
int     r2 = 0;
char    tempRead1[255];
char    tempRead2[255];

void write_to_file(int f, const char* p_data, int size) {
    write(f, p_data, size);
}

void read_from_file_print1(int f, int size) {
    read(f, tempRead1, size);
    printf("%s\n", tempRead1);
}

void read_from_file_print2(int f, int size) {
    read(f, tempRead2, size);
    printf("%s\n", tempRead2);
}

void seek_data(int f, int place) {
    lseek(f, place, SEEK_SET);
}

int main() {
    readWriteFile = open(FILE_NAME, O_RDWR);
    r2 = open(FILE_NAME, O_RDWR);

    ioctl(readWriteFile, 1, 200);

    write_to_file(readWriteFile, "a", 1);
    write_to_file(r2, "B", 1);

    seek_data(readWriteFile, 0);
    seek_data(r2, 0);

    read_from_file_print1(readWriteFile, 1);
    read_from_file_print2(r2, 1);

    seek_data(readWriteFile, 6);
    seek_data(r2, 3);

    write_to_file(readWriteFile, "c", 1);
    write_to_file(r2, "D", 1);

    seek_data(readWriteFile, 6);
    seek_data(r2, 3);

    read_from_file_print1(readWriteFile, 1);
    read_from_file_print2(r2, 1);

    close(readWriteFile);
    close(r2);

    return 0;
}