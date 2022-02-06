#include <string.h>
#include <stdio.h>
#include <time.h>

#include "crypt.h"

/* create not-encrypted message file */
_Bool create_file() {
    time_t t;   // not a primitive datatype
    time(&t);
    unsigned char string[10000];
    for (int i = 0; i < 10000; i++) {
        string[i] = '\0';
    }
    printf("\033[1;33m"); // yellow
    fgets(string, 10000, stdin);
    printf("  -- %s",ctime(&t));
    printf("\n\n");
    printf("\033[0m"); // default color
    FILE *raw_file = fopen("/tmp/message.txt", "w");
    int i = 0;
    fputs(string, raw_file);
    fclose(raw_file);
    if (strcmp(string,"!exit\n") == 0) {
        return 1;
    }
    return 0;
}

/* generate message for communication establishment and key check */
_Bool create_init_file() {
    char str[10000];
    for (int i = 0; i < 10000; i++) {
        str[i] = '\0';
    }
    strcpy(str,"Super kouzelna zprava pro draka!");
    FILE *raw_file = fopen("/tmp/message.txt", "w");
    int i = 0;
    fputs(str, raw_file);
    fclose(raw_file);
    return 0;
}

/* get length of file password */
int passwd_len(char text[])
{
    char c = 'x';
    int i = 0;

    while (c != '\0') {
        c = text[i];
        i++;
    }

    return (i - 1);
}


/* encrypt file with KEY password */
_Bool encrypt_file(int KEY) {
    char *file = "/tmp/message.txt";
    char *newfile = "/tmp/encrypted_message.txt";

    /* int password to string */
    char passwd_text[7];
    passwd_text[0] = KEY/1000000;
    passwd_text[1] = (KEY/100000)%10;
    passwd_text[2] = (KEY/10000)%10;
    passwd_text[3] = (KEY/1000)%10;
    passwd_text[4] = (KEY/100)%10;
    passwd_text[5] = (KEY/10)%10;
    passwd_text[6] = KEY%10;

    FILE *inputFile;
    FILE *outputFile;
    inputFile = fopen(file, "r");
    outputFile = fopen(newfile, "w");

    fprintf(outputFile, file);	// original file name
    fputc('2', outputFile);
    fputc('\n', outputFile);
    fprintf(outputFile, "caesar\n");	// encoding type
    fprintf(outputFile, "štěpínův úžasný zakódovaný soubor  :)\n");	// random mess

    int passwd = passwd_len(passwd_text);
    unsigned char ch;
    unsigned long long position = 1;

    while (!feof(inputFile)) {
        ch = fgetc(inputFile);
        if (feof(inputFile)) {
            break;
        }

        int ch_int = ch;
        unsigned char ch = ch_int;

        for (int i = (passwd - 2); i >= 0; i--) {
            if (position % (i + 1) == 0) {
                ch_int = ch_int + (int)passwd_text[i];
                ch = ch_int;
                break;
            }
        }

        position++;
        fputc(ch, outputFile);
    }

    fclose(inputFile);
    fclose(outputFile);

    return 0;
}

/* Decode encrypted file */
_Bool decrypt_file(int KEY)
{
    char *file = "/tmp/message_received.txt";
    char passwd_text[7];
    passwd_text[0] = KEY/1000000;
    passwd_text[1] = (KEY/100000)%10;
    passwd_text[2] = (KEY/10000)%10;
    passwd_text[3] = (KEY/1000)%10;
    passwd_text[4] = (KEY/100)%10;
    passwd_text[5] = (KEY/10)%10;
    passwd_text[6] = KEY%10;

    FILE *inputFile;
    inputFile = fopen(file, "r");

    char newfile[100];	// get name of original file
    fgets(newfile, 100, inputFile);
    newfile[strcspn(newfile, "\n")] = 0;

    FILE *outputFile;
    outputFile = fopen(newfile, "w");

    char encryptionType[20];	// encoding type
    fgets(encryptionType, 20, inputFile);

    if (!
            (encryptionType[0] == 'c' && encryptionType[1] == 'a'
             && encryptionType[2] == 'e' && encryptionType[3] == 's'
             && encryptionType[4] == 'a' && encryptionType[5] == 'r')) {
        printf("cypher not recognised\n");
        return 100;
    }

    char mess[200];
    fgets(mess, 200, inputFile);	// random mess

    int passwd = passwd_len(passwd_text);
    char ch;
    unsigned long long position = 1;

    while (!feof(inputFile)) {
        ch = fgetc(inputFile);
        if (feof(inputFile)) {
            break;
        }

        int ch_int = ch;
        char ch = ch_int;

        for (int i = passwd - 2; i >= 0; i--) {
            if (position % (i + 1) == 0) {
                ch_int = ch_int - (int)passwd_text[i];
                ch = ch_int;
                break;
            }
        }

        position++;
        fputc(ch, outputFile);
    }

    fclose(inputFile);
    fclose(outputFile);

    return 0;
}

void create_file_received(unsigned char *string) {
    FILE *raw_file = fopen("/tmp/message_received.txt", "w");
    int i = 0;
    char charakter = string[i];
    while (charakter != '\0') {
        i++;
        putc(charakter,raw_file);
        charakter = string[i];
    }
    fclose(raw_file);
}


_Bool read_received_message() {
    time_t t;   // not a primitive datatype
    time(&t);

    FILE *message = fopen("/tmp/message.txt2", "r");
    unsigned char ch;
    printf("\033[1;32m"); // green
    printf("[ * ] ");
    int i = 0;
    char str[6];
    while (!feof(message)) {
        ch = fgetc(message);
        if (i < 5) {
            str[i] = ch;
        }
        if (feof(message)) {
            break;
        }
        printf("%c", ch);
        i++;
    }
    printf("  -- %s\n",ctime(&t));
    printf("\033[1;33m"); // yellow
    printf("\n");
    str[5] = '\0';
    if (str[0] == '!' && str[1] == 'e' && str[2] == 'x' && str[3] == 'i' && str[4] == 't') {

        fprintf(stderr, "Disconnected\n");
        printf("\033[0m"); // default color
        return 1;
    }
    return 0;
}

_Bool read_received_init_message() {
    FILE *message = fopen("/tmp/message.txt2", "r");
    unsigned char ch;
    int i = 0;
    char str[32];
    while (!feof(message)) {
        ch = fgetc(message);
        if (i < 32) {
            str[i] = ch;
        }
        if (feof(message)) {
            break;
        }
        i++;
    }
    if (str[0] == 'S' && str[1] == 'u' && str[2] == 'p' && str[3] == 'e' && str[4] == 'r' && str[5] == ' ' && str[6] == 'k' && str[7] == 'o' && str[8] == 'u' && str[9] == 'z' && str[10] == 'e' && str[11] == 'l' && str[12] == 'n' && str[14] == ' ' && str[15] == 'z' && str[16] == 'p' && str[17] == 'r'&& str[19] == 'v' && str[20] == 'a' && str[21] == ' '&& str[31] == '!') {
        return 1;
    }
    return 0;
}