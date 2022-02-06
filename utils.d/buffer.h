#ifndef DEFFIE_HELLMAN_BUFFER_H
#define DEFFIE_HELLMAN_BUFFER_H

long long int buffer_to_lld(unsigned char *buffer);

void load_buffer(long long int long_1, unsigned char *hello, int hello_size);

void load_buffer_message(char *file_adr, unsigned char *hello, int hello_size);

#endif
