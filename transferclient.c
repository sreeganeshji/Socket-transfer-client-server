#include <string.h>
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
    "  transferclient [options]\n"                           \
    "options:\n"                                             \
    "  -s                  Server (Default: localhost)\n"    \
    "  -p                  Port (Default: 19121)\n"           \
    "  -o                  Output file (Default cs6200.txt)\n" \
    "  -h                  Show this help message\n"

/* OPTIONS DESCRIPTOR ====================================================== */
static struct option gLongOptions[] = {
    {"server", required_argument, NULL, 's'},
    {"port", required_argument, NULL, 'p'},
    {"output", required_argument, NULL, 'o'},
    {"help", no_argument, NULL, 'h'},
    {NULL, 0, NULL, 0}};

/* Main ========================================================= */
int main(int argc, char **argv)
{
    int option_char = 0;
    char *hostname = "localhost";
    unsigned short portno = 19121;
    char *filename = "cs6200.txt";

    setbuf(stdout, NULL);

    // Parse and set command line arguments
    while ((option_char = getopt_long(argc, argv, "s:p:o:hx", gLongOptions, NULL)) != -1)
    {
        switch (option_char)
        {
        case 's': // server
            hostname = optarg;
            break;
        case 'p': // listen-port
            portno = atoi(optarg);
            break;
        default:
            fprintf(stderr, "%s", USAGE);
            exit(1);
        case 'o': // filename
            filename = optarg;
            break;
        case 'h': // help
            fprintf(stdout, "%s", USAGE);
            exit(0);
            break;
        }
    }

    if (NULL == hostname)
    {
        fprintf(stderr, "%s @ %d: invalid host name\n", __FILE__, __LINE__);
        exit(1);
    }

    if (NULL == filename)
    {
        fprintf(stderr, "%s @ %d: invalid filename\n", __FILE__, __LINE__);
        exit(1);
    }

    if ((portno < 1025) || (portno > 65535))
    {
        fprintf(stderr, "%s @ %d: invalid port number (%d)\n", __FILE__, __LINE__, portno);
        exit(1);
    }

    /* Socket Code Here */
    /*
     1. create addrinfo and get the address using the domain and service name.
     2. create a socket.
     3. connect the socket to the address.
     4. receive data.
     */
    
    struct addrinfo addressHints, *clientAddress;
    memset(&addressHints, 0, sizeof(addressHints));
    addressHints.ai_family = AF_UNSPEC;
    addressHints.ai_socktype = SOCK_STREAM;
    addressHints.ai_flags = AI_PASSIVE;
    
    char portNoChar[20];
    snprintf(portNoChar, 20, "%d",portno);
    
    int getAddressStatus = getaddrinfo(hostname, portNoChar, &addressHints, &clientAddress);
    
    if (getAddressStatus != 0)
    {
        fprintf(stderr,"couldn't obtain the address\n");
        exit(1);
    }
    
    int clientSocket = socket(clientAddress->ai_family, clientAddress->ai_socktype, 0);
    
    int connectionStatus = connect(clientSocket, clientAddress->ai_addr, clientAddress->ai_addrlen);
    if(connectionStatus!=0)
    {
        fprintf(stderr,"couldn't connect to the address\n");
        exit(1);
    }
    FILE* clientFile = fopen(filename, "w+");
    char buffer[BUFSIZE];
    
    memset(buffer,0,sizeof(buffer));

    ssize_t receivedBytes = recv(clientSocket, buffer, BUFSIZE, 0);
    
    while(receivedBytes>0){
        fwrite(buffer, sizeof(char), receivedBytes, clientFile);
        memset(buffer, 0, sizeof(buffer));
        receivedBytes = recv(clientSocket, buffer, BUFSIZE, 0);
    }
    
    fclose(clientFile);
    freeaddrinfo(clientAddress);
    close(clientSocket);
    
    return 0;

}

