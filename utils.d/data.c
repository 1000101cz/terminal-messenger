#include <stdio.h>
#include <stdlib.h>

#include "data.h"

char number_to_char(long long int x) {
    if (x == 0) {
        return '0';
    } else if (x == 1) {
        return '1';
    } else if (x == 2) {
        return '2';
    } else if (x == 3) {
        return '3';
    } else if (x == 4) {
        return '4';
    } else if (x == 5) {
        return '5';
    } else if (x == 6) {
        return '6';
    } else if (x == 7) {
        return '7';
    } else if (x == 8) {
        return '8';
    } else if (x == 9) {
        return '9';
    }
    return '0';
}

long long int char_to_number(char x) {
    if (x == '0') {
        return 0;
    } else if (x == '1') {
        return 1;
    } else if (x == '2') {
        return 2;
    } else if (x == '3') {
        return 3;
    } else if (x == '4') {
        return 4;
    } else if (x == '5') {
        return 5;
    } else if (x == '6') {
        return 6;
    } else if (x == '7') {
        return 7;
    } else if (x == '8') {
        return 8;
    } else if (x == '9') {
        return 9;
    }
    return 0;
}
