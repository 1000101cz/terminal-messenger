#ifndef DIFFIEHELLMAN_CRYPT_H
#define DIFFIEHELLMAN_CRYPT_H

_Bool create_file();

_Bool create_init_file();

int passwd_len(char text[]);

_Bool encrypt_file(int KEY);

_Bool decrypt_file(int KEY);

void create_file_received(unsigned char *string);

_Bool read_received_message();

_Bool read_received_init_message();

#endif //DIFFIEHELLMAN_CRYPT_H
