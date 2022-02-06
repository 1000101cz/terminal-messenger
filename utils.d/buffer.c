#include <stdlib.h>
#include <stdio.h>

#include "buffer.h"
#include "data.h"

long long int buffer_to_lld(unsigned char *buffer) {
    int i = 0;
    int ii = 1;
    long long int BC_long = 0;
    while (1) {
        if (buffer[i] == '\0') {
            break;
        }
        BC_long = BC_long + ii * char_to_number(buffer[i]);
        ii = ii * 10;
        i++;
    }
    return BC_long;
}

void load_buffer(long long int long_1, unsigned char *hello, int hello_size) {
    long long int long_2 = long_1;
    int i = 0;
    int ii = 10;
    for (int j = 0; j < hello_size; j++) {
        hello[j] = '\0';
    }
    while(long_2 > 0) {
        hello[i] = number_to_char((long_2 % ii) / (ii/10));
        long_2 = long_2 - long_2 % ii;
        ii=ii*10;
        i++;
    }
}

void load_buffer_message(char *file_adr, unsigned char *hello, int hello_size) {
    for (int j = 0; j < hello_size; j++) {
        hello[j] = '\0';
    }

    int position = 0;
    FILE *inputFile = fopen(file_adr, "r");
    unsigned char ch;

    while (!feof(inputFile)) {
        ch = fgetc(inputFile);
        if (feof(inputFile)) {
            break;
        }
        hello[position] = ch;
        position++;
    }
}
