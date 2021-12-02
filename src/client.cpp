#include <netdb.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#define MAX_INPUT_SIZE 4096
#ifdef WIN32
#include <windows.h>
#elif _POSIX_C_SOURCE >= 199309L
#include <time.h> // for nanosleep
#else
#include <unistd.h> // for usleep
#endif

void sleep_ms(int milliseconds)
{ // cross-platform sleep function
#ifdef WIN32
    Sleep(milliseconds);
#elif _POSIX_C_SOURCE >= 199309L
    struct timespec ts;
    ts.tv_sec = milliseconds / 1000;
    ts.tv_nsec = (milliseconds % 1000) * 1000000;
    nanosleep(&ts, NULL);
#else
    if (milliseconds >= 1000)
        sleep(milliseconds / 1000);
    usleep((milliseconds % 1000) * 1000);
#endif
}

int main(int argc, char *argv[])
{
    time_t t;
    /* Intializes random number generator */
    srand((unsigned)time(&t));
    clock_t ti, tf;
    int sockfd, portnum, n;
    struct sockaddr_in server_addr;
    char inputbuf[MAX_INPUT_SIZE];
    if (argc < 3)
    {
        fprintf(stderr, "usage %s <server-ip-addr> <server-port>\n", argv[0]);
        exit(0);
    }
    portnum = atoi(argv[2]);
    /* Create client socket */
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        fprintf(stderr, "ERROR opening socket\n");
        exit(1);
    }
    /* Fill in server address */
    bzero((char *)&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    if (!inet_aton(argv[1], &server_addr.sin_addr))
    {
        fprintf(stderr, "ERROR invalid server IP address\n");
        exit(1);
    }
    server_addr.sin_port = htons(portnum);
    /* Connect to server */
    int x = connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr));
    if (x < 0)
    {
        fprintf(stderr, "ERROR connecting / Server not Active / Server Busy\n");
        exit(1);
    }
    printf("Connected to server \n");
    do
    {
        /* Ask user for message to send to server */
        // sleep_ms(rand() % 10000);
        printf("Please enter the message to the server: \n");
        bzero(inputbuf, MAX_INPUT_SIZE);
        fgets(inputbuf, MAX_INPUT_SIZE - 1, stdin);
        ti = clock();
        n = write(sockfd, inputbuf, strlen(inputbuf));
        if (n < 0)
        {
            fprintf(stderr, "ERROR writing to socket\n");
            exit(1);
        }
        /* Read reply */
        bzero(inputbuf, MAX_INPUT_SIZE);
        n = read(sockfd, inputbuf, (MAX_INPUT_SIZE - 1));
        if (n < 0)
        {
            fprintf(stderr, "ERROR reading from socket\n");
            exit(1);
        }
        printf("Server replied:\n%s\n", inputbuf);
        tf = clock();
        // printf("Time %f sec\n", ((double)(tf - ti)) / CLOCKS_PER_SEC);
    } while (1);
    return 0;
}