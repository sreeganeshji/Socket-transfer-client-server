#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <netdb.h>
#include <getopt.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>

#define BUFSIZE 1219

#define USAGE                                                \
    "usage:\n"                                               \
    "  transferserver [options]\n"                           \
    "options:\n"                                             \
    "  -f                  Filename (Default: 6200.txt)\n" \
    "  -h                  Show this help message\n"         \
    "  -p                  Port (Default: 19121)\n"

/* OPTIONS DESCRIPTOR ====================================================== */
static struct option gLongOptions[] = {
    {"filename", required_argument, NULL, 'f'},
    {"help", no_argument, NULL, 'h'},
    {"port", required_argument, NULL, 'p'},
    {NULL, 0, NULL, 0}};

int main(int argc, char **argv)
{
    int option_char;
    int portno = 19121;             /* port to listen on */
    char *filename = "6200.txt"; /* file to transfer */

    setbuf(stdout, NULL); // disable buffering

    // Parse and set command line arguments
    while ((option_char = getopt_long(argc, argv, "p:hf:x", gLongOptions, NULL)) != -1)
    {
        switch (option_char)
        {
        case 'p': // listen-port
            portno = atoi(optarg);
            break;
        default:
            fprintf(stderr, "%s", USAGE);
            exit(1);
        case 'h': // help
            fprintf(stdout, "%s", USAGE);
            exit(0);
            break;
        case 'f': // file to transfer
            filename = optarg;
            break;
        }
    }


    if ((portno < 1025) || (portno > 65535))
    {
        fprintf(stderr, "%s @ %d: invalid port number (%d)\n", __FILE__, __LINE__, portno);
        exit(1);
    }
    
    if (NULL == filename)
    {
        fprintf(stderr, "%s @ %d: invalid filename\n", __FILE__, __LINE__);
        exit(1);
    }

    /* Socket Code Here */
    /*
     1. create hint and addressinfo
     2. get the addressinfo using the port number
     3. create the socket
     4. bind the socket to the address obtained in step 2
     5. listen for the connection using this socket
     6. accept any connection and create a new socket.
     7. transfer the file in chunks and repeat till the end
     */

    
    struct addrinfo hint, *socketAddress;
    memset(&hint, 0, sizeof(hint));
    
    hint.ai_family = AF_UNSPEC;
    hint.ai_socktype = SOCK_STREAM;
    hint.ai_flags = AI_PASSIVE;
    
    char portNumberChar[20];
    snprintf(portNumberChar, 20, "%d",portno);
    
    int gettingAddressStatus = getaddrinfo(NULL, portNumberChar, &hint, &socketAddress);
    if (gettingAddressStatus != 0)
    {
        fprintf(stderr, "could not get address form the port number\n");
        exit(1);
    }
    
    int serverSocket = socket(socketAddress->ai_family, socketAddress->ai_socktype, 0);
    int serverSocketOption = 1;
    int serverSocketOptionStatus = setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &serverSocketOption, sizeof(int));
    
    if(serverSocketOptionStatus != 0)
    {
        fprintf(stderr, "couldn't print into the socket\n");
        exit(1);
    }
    
    int serverSocketBindStatus = bind(serverSocket, socketAddress->ai_addr, socketAddress->ai_addrlen);
    if(serverSocketBindStatus!=0)
    {
        fprintf(stderr, "couldn't bind the socket\n");
        exit(1);
    }
    int listeningStatus = listen(serverSocket, 1);
    if(listeningStatus!=0)
    {
        fprintf(stderr,"couldn't listen\n");
        exit(1);
    }
    
    FILE * serverFile = fopen(filename, "r");
    char buffer[BUFSIZE];
    memset(buffer,0,sizeof(buffer));
    
    while (1) { //continue to accept connections from other clients to send data to.
        fseek(serverFile, 0, SEEK_SET);
    int newSocket = accept(serverSocket, socketAddress->ai_addr, &socketAddress->ai_addrlen);
    
        memset(buffer, 0, sizeof(buffer));
        char* fgetsStatus = fgets(buffer, BUFSIZE, serverFile);

    while(fgetsStatus != 0){
        unsigned long leftToBeSent = strlen(buffer);
        int sent = 0;
        while(leftToBeSent > 0)
        {
            ssize_t sentSize = send(newSocket, buffer+sent, leftToBeSent, 0);
            if(sentSize == 0)
            {
                break;
            }
            sent += sentSize;
            leftToBeSent -= sentSize;
        }
        memset(buffer, 0, sizeof(buffer));
      fgetsStatus = fgets(buffer, BUFSIZE, serverFile);
    }
 
        close(newSocket);
        memset(buffer,0,sizeof(buffer));
    }
    return 0;
}
