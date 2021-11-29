#include <netdb.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <errno.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/time.h>
#include <limits.h>
#include <math.h>
#define MAX_INPUT_SIZE 256

void handleInput(char *input, char *arr)
{
    if (input[0] == '0')
    {
        sprintf(arr, "%s", "\nWelcome to NormDB\n\tPress 1 for Insert Table\n\tPress 2 for Drop Table\n\tPress 3 for Insert\n\tPress 4 for Update\n\tPress 5 for Delete\n\tPress 6 to Show\n");
    }
    else if (input[1] == '1')
    {
        sprintf(arr, "%s", "\nTable Added\n");
    }
    else if (input[1] == '2')
    {
        sprintf(arr, "%s", "\nTable Dropped\n");
    }
    else if (input[1] == '3')
    {
        sprintf(arr, "%s", "\nRow Added\n");
    }
    else
    {
        sprintf(arr, "%s", "\nID\t Name\t Email\tPhone No.\n1\tabcd\tabcd@email.com\t1234567890\n2\taafd\ta34cd@email.com\t223344567890\n3\tabad\tasacd@email.com\t123453290\n");
    }
}

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        fprintf(stderr, "usage %s <server-port>\n", argv[0]);
        exit(0);
    }

    int opt = 1;
    int master_socket, addrlen, new_socket, client_socket[30], max_clients = 30, activity, i, valread, sd;
    int max_sd;
    int PORT = atoi(argv[1]);
    struct sockaddr_in address;

    char buffer[MAX_INPUT_SIZE]; // data buffer of 256
    char *out = (char *)malloc(sizeof(char) * 10);

    // set of socket descriptors
    fd_set readfds;

    // initialise all client_socket[] to 0 so not checked
    for (i = 0; i < max_clients; i++)
    {
        client_socket[i] = 0;
    }

    // create a master socket
    if ((master_socket = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // set master socket to allow multiple connections ,
    // this is just a good habit, it will work without this
    if (setsockopt(master_socket, SOL_SOCKET, SO_REUSEADDR, (char *)&opt,
                   sizeof(opt)) < 0)
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    // type of socket created
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // bind the socket to localhost port 8888
    if (bind(master_socket, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    printf("Listener on port %d \n", PORT);

    // try to specify maximum of 3 pending connections for the master socket
    if (listen(master_socket, 3) < 0)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    // accept the incoming connection
    addrlen = sizeof(address);
    puts("Waiting for connections ...");

    while (1)
    {
        // clear the socket set
        FD_ZERO(&readfds);

        // add master socket to set
        FD_SET(master_socket, &readfds);
        max_sd = master_socket;

        // add child sockets to set
        for (i = 0; i < max_clients; i++)
        {
            // socket descriptor
            sd = client_socket[i];

            // if valid socket descriptor then add to read list
            if (sd > 0)
                FD_SET(sd, &readfds);

            // highest file descriptor number, need it for the select function
            if (sd > max_sd)
                max_sd = sd;
        }

        // wait for an activity on one of the sockets , timeout is NULL ,
        // so wait indefinitely
        activity = select(max_sd + 1, &readfds, NULL, NULL, NULL);

        if ((activity < 0) && (errno != EINTR))
        {
            printf("select error");
        }

        // If something happened on the master socket ,
        // then its an incoming connection
        if (FD_ISSET(master_socket, &readfds))
        {
            if ((new_socket = accept(master_socket,
                                     (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0)
            {
                perror("accept");
                exit(EXIT_FAILURE);
            }

            // inform user of socket number - used in send and receive commands
            printf("New connection with socket fd is %d  \n", new_socket);

            // add new socket to array of sockets
            for (i = 0; i < max_clients; i++)
            {
                // if position is empty
                if (client_socket[i] == 0)
                {
                    client_socket[i] = new_socket;
                    printf("Adding to list of sockets as %d\n", i);

                    break;
                }
            }
        }

        // else its some IO operation on some other socket
        for (i = 0; i < max_clients; i++)
        {
            sd = client_socket[i];

            if (FD_ISSET(sd, &readfds))
            {
                valread = read(sd, buffer, MAX_INPUT_SIZE);
                if (valread == 0)
                {
                    printf("Client Socket %d disconnected\n", i);
                    close(sd);
                    client_socket[i] = 0;
                    continue;
                }
                printf("Client Socket %d sent a message : ", i);
                for (int i = 0; buffer[i] != '\n'; i++)
                    printf("%c", buffer[i]);
                printf("\n");

                handleInput(buffer, out);

                printf("Sending Reply to Client %d: %s\n", i, out);

                send(sd, out, strlen(out), 0);
            }
        }
    }
    return 0;
}
