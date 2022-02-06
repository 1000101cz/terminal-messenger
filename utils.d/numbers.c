#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include "numbers.h"

long long int gen_number(int s) {
    long long int read_from_file;
    srand(time(0) + s);
    int r = rand() % 586080;
    FILE *primes = fopen("data.d/primes.txt", "r");
    for (int i = 0; i < r; i++) {
        fscanf(primes,"%lld\n",&read_from_file);
    }
    (void) fclose(primes);
    return read_from_file;
}
