// PC1 starts communication
/* A - PC1 private key
 * B - PC2 private key
 * C - public key
 */
/* Steps:
 *   1) generate A (private) and C (public)
 *   2) create A-C combination
 *   3) send shared C number to PC2
 *   4) send public A-C number to PC2
 *   5) receive B-C number from PC2
 *   6) create A-B-C key from B-C and A
 */

#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <pthread.h>

#include "../utils.d/numbers.h"
#include "../utils.d/buffer.h"
#include "../utils.d/crypt.h"

#define PORT 8080

#define BUFFER_SIZE 1000

void *receive_thread(void *);

int sock = 0;
int server_fd, new_socket, valread;

int ABC; // private

int main() {
    enum { INPUT, NUM_THREADS };  //create threads
    void *(*thr_functions[])(void *) = { receive_thread };
    pthread_t threads[NUM_THREADS];

    /* start communication with PC1 */

    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    unsigned char buffer[BUFFER_SIZE*BUFFER_SIZE] = {0};
    char hello[BUFFER_SIZE*BUFFER_SIZE] = "Hello from PC2";

    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Forcefully attaching socket to the port 8080
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons( PORT );

    // Forcefully attaching socket to the port 8080
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address))<0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    if (listen(server_fd, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }
    if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen))<0) {
        perror("accept");
        exit(EXIT_FAILURE);
    }
    valread = read( new_socket , buffer, BUFFER_SIZE*BUFFER_SIZE);
    send(new_socket , hello , strlen(hello) , 0 );
    printf("\033[1;34m"); // blue
    printf("\t\tTerminal Messenger");
    printf("\033[0;34m"); // blue
    printf(" \t(Stepan Marousek | 2022)\n\n");
    printf("\033[0m");

    long long int B; // private
    long long int C = 0; // public
    long long int AC = 0; // public
    long long int BC; // test
    long long int ABC_long; // private

    int try_number = 0;
    while(1) {
        try_number++;

        /* wait for C and AC from PC1 */
        for (int ttt = 0; ttt < BUFFER_SIZE*BUFFER_SIZE; ttt++) { buffer[ttt] = '\0'; }
        valread = read( new_socket , buffer, BUFFER_SIZE*BUFFER_SIZE); // read C
        C = buffer_to_lld(buffer);


        for (int ttt = 0; ttt < BUFFER_SIZE*BUFFER_SIZE; ttt++) { buffer[ttt] = '\0'; }
        valread = read( new_socket , buffer, BUFFER_SIZE*BUFFER_SIZE); // read AC
        AC = buffer_to_lld(buffer);


        /* generate B number */
        B = gen_number(-323521548 + try_number);


        /* create BC combination */
        BC = B * C;
        while (BC >= 10000000) { BC = BC - 10000000; }


        /* send BC to PC1 */
        load_buffer(BC, hello, BUFFER_SIZE*BUFFER_SIZE);
        send(new_socket , hello , strlen(hello) , 0 ); // send BC
        // BC sent


        /* create ABC key from AC abd B */
        ABC_long = AC * B;
        while (ABC_long >= 10000000) {
            ABC_long = ABC_long - 10000000;
            if (ABC_long < 1000000) {
                ABC_long = ABC_long + 1000000;
            }
        }
        ABC = ABC_long;

        for (int ttt = 0; ttt < BUFFER_SIZE*BUFFER_SIZE; ttt++) { buffer[ttt] = '\0'; }
        valread = read(new_socket, buffer, BUFFER_SIZE*BUFFER_SIZE); // read AC
        create_file_received(buffer);
        decrypt_file(ABC);
        if (read_received_init_message()) {

            create_init_file();
            encrypt_file(ABC);
            load_buffer_message("/tmp/encrypted_message.txt", hello, BUFFER_SIZE*BUFFER_SIZE);
            send(new_socket, hello, strlen(hello), 0);

            printf("Secure connection established\n");
            printf("KEY = %d\n\n",ABC);
            break;
        } else {
            create_init_file();
            encrypt_file(ABC);
            load_buffer_message("/tmp/encrypted_message.txt", hello, BUFFER_SIZE*BUFFER_SIZE);
            send(new_socket, hello, strlen(hello), 0);
        }
    }

    pthread_create(&threads[0], NULL, thr_functions[0], NULL);

    while (1) {
        /* create, prepare and send message to PC2*/
        if (create_file()) {
            encrypt_file(ABC);
            load_buffer_message("/tmp/encrypted_message.txt", hello, BUFFER_SIZE*BUFFER_SIZE);
            send(new_socket, hello, strlen(hello), 0); // send C
            break;
        }
        encrypt_file(ABC);
        load_buffer_message("/tmp/encrypted_message.txt", hello, BUFFER_SIZE*BUFFER_SIZE);
        send(new_socket, hello, strlen(hello), 0); // send C
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
        valread = read(new_socket, buffer, BUFFER_SIZE*BUFFER_SIZE); // read AC
        printf("\a");
        create_file_received(buffer);
        decrypt_file(ABC);
        if (read_received_message()) {
            break;
        }
    }
    return NULL;
}
