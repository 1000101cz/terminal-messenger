// PC1 - client
// Diffie - Hellman key exchange

/* A - PC1 private key
 * B - PC2 private key
 * C - public key
 * ABC - private key - same on PC1 and PC2
 */

/* Steps:
 *   1) generate A (private) and C (public)
 *   2) create A-C combination
 *   3) send shared C number to PC2
 *   4) send public A-C number to PC2
 *   5) receive B-C number from PC2
 *   6) create A-B-C key from B-C and A
 */

#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>

#include "../utils.d/numbers.h"
#include "../utils.d/buffer.h"
#include "../utils.d/crypt.h"

#define PORT 8080

#define BUFFER_SIZE 1000

void *receive_thread(void *);

int sock, valread;

int ABC; // private

int main(int argc, char *argv[]) {
    fprintf(stderr, "\033[1;34m"); // blue

    enum { INPUT, NUM_THREADS };  //create threads
    void *(*thr_functions[])(void *) = { receive_thread };
    pthread_t threads[NUM_THREADS];

    if (argc == 1) {
      fprintf(stderr, "ERROR: enter IP address of server PC!\n");
    }

    /* start communication with PC2 */

    struct sockaddr_in serv_addr;
    unsigned char hello[BUFFER_SIZE*BUFFER_SIZE] = "Hello from PC1\0";
    unsigned char buffer[BUFFER_SIZE*BUFFER_SIZE] = {0};
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\n Socket creation error \n");
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // Convert IPv4 and IPv6 addresses from text to binary form
    if(inet_pton(AF_INET, argv[1], &serv_addr.sin_addr)<=0) {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        printf("\nConnection Failed \n");
        return -1;
    }
    send(sock , hello , strlen(hello) , 0 );
    valread = read( sock , buffer, BUFFER_SIZE*BUFFER_SIZE);
    printf("\033[1;34m"); // blue
    printf("\t\tTerminal Messenger");
    printf("\033[0;34m"); // blue
    printf(" \t(Stepan Marousek | 2022)\n\n");
    printf("\033[0m");

    long long int A; // private
    long long int C; // public
    long long int AC; // public
    long long int BC; // test
    long long int ABC_long; // private

    int try_number = 0;
    while(1) { // find valid key
        try_number++;

        /* generate A and C numbers */
        A = gen_number(0 + try_number); // A created
        C = gen_number(23 + try_number); // C created


        /* create AC combination */
        AC = A * C;
        while (AC >= 10000000) { AC = AC - 10000000; } // AC created

        /* send C to PC2 */
        load_buffer(C, hello, BUFFER_SIZE*BUFFER_SIZE);
        send(sock , hello , strlen(hello) , 0 ); // send C


        /* send AC to PC2 */
        load_buffer(AC, hello, BUFFER_SIZE*BUFFER_SIZE);
        send(sock , hello , strlen(hello) , 0 ); // send AC


        /* wait for BC combination from PC2 */
        for (int ttt = 0; ttt < BUFFER_SIZE*BUFFER_SIZE; ttt++) { buffer[ttt] = '\0'; }  // clear buffer
        valread = read( sock , buffer, sizeof(char)*100); // wait for BC
        BC = buffer_to_lld(buffer);  // turn received string into number


        /* create ABC key from BC and A */
        ABC_long = BC * A;
        while (ABC_long >= 10000000) {
            ABC_long = ABC_long - 10000000;
            if (ABC_long < 1000000) {
                ABC_long = ABC_long + 1000000;
            }
        }
        ABC = ABC_long;

        create_init_file();
        encrypt_file(ABC);
        load_buffer_message("/tmp/encrypted_message.txt", hello, BUFFER_SIZE*BUFFER_SIZE);
        send(sock, hello, strlen(hello), 0);

        for (int ttt = 0; ttt < BUFFER_SIZE*BUFFER_SIZE; ttt++) { buffer[ttt] = '\0'; }
        valread = read(sock, buffer, BUFFER_SIZE*BUFFER_SIZE); // read AC
        create_file_received(buffer);
        decrypt_file(ABC);
        if (read_received_init_message()) {
            printf("Secure connection established\n");
            printf("KEY = %d\n\n",ABC);
            break;
        }
    }

    pthread_create(&threads[0], NULL, thr_functions[0], NULL);

    while (1) {
        /* create, prepare and send message to PC2*/
        if (create_file()) {
            encrypt_file(ABC);
            load_buffer_message("/tmp/encrypted_message.txt", hello, BUFFER_SIZE*BUFFER_SIZE);
            send(sock, hello, strlen(hello), 0); // send C
            break;
        }
        encrypt_file(ABC);
        load_buffer_message("/tmp/encrypted_message.txt", hello, BUFFER_SIZE*BUFFER_SIZE);
        send(sock, hello, strlen(hello), 0); // send C
    }
    pthread_cancel(threads[0]);
    pthread_join(threads[0], NULL);

    return 0;
}

/* thread handeling incoming messages */
void *receive_thread(void *d)
{
    unsigned char buffer[BUFFER_SIZE*BUFFER_SIZE] = {0};
    while(1) {
        for (int ttt = 0; ttt < BUFFER_SIZE*BUFFER_SIZE; ttt++) { buffer[ttt] = '\0'; }
        valread = read(sock, buffer, BUFFER_SIZE*BUFFER_SIZE); // read AC
        printf("\a");
        create_file_received(buffer);
        decrypt_file(ABC);
        if (read_received_message()) {
            break;
        }
    }
    return NULL;
}
