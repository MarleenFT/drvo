#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h> 
#include <sys/types.h> 
#include <fcntl.h> 

#define FILE_NAME   "/dev/drvo_device"
#define MAX_SIZE    255

int readWriteFile = 0;
char tempRead[255];

void write_to_file(const char* p_data, int size) {
    readWriteFile = open(FILE_NAME, O_WRONLY);
    write(readWriteFile, p_data, size);
    close(readWriteFile);
}

void read_from_file_print() {
    readWriteFile = open(FILE_NAME, O_RDONLY);
    read(readWriteFile, tempRead, sizeof(tempRead));
    printf("%s\r\n", tempRead);
    close(readWriteFile);
}

int main() {
    write_to_file("a", 2);
    read_from_file_print();

    return 0;
}