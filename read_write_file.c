#include <stdio.h>
#include <stdlib.h>

#define FILE_NAME   "/dev/drvo_practica"
#define MAX_SIZE    255

void write_to_file(const char* p_data) {
    FILE* fp = fopen(FILE_NAME, "w");

    fprintf(fp, "%s", p_data);

    fclose(fp);
}

void read_from_file_print() {
    FILE* fp = fopen(FILE_NAME, "r");
    char ch = ' ';

    while((ch = getc(fp)) != EOF)
        printf("%c",ch);

    fclose(fp);
}

int main() {
    write_to_file("AAAAAAAAAAAAAAAAAAA\n");
    read_from_file_print();

    return 0;
}