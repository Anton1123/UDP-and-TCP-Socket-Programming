#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/time.h> //for function get time of day.

#define PORT 3490 // the port client will be connecting to
#define TRIALS 17 //number of times i will ask for connection (-1) from server. 17 because 2^16 = 65536
#define MAXDATASIZE 65535 // max number of bytes we can get at once (TCP max packet size)

int main(int argc, char *argv[])
{
    int sockfd, numbytes;
    char buf[MAXDATASIZE];
    struct hostent *he;
    struct sockaddr_in their_addr; // connector's address information

    struct timeval tval_after; // for getting current time

    FILE *f = fopen("rcvtimesTCP.txt", "w"); //files which will hold time packets arrived. //make sure this file is in the same directory.
    if (f == NULL)
    {
        printf("Error opening file!\n");
        exit(1);
    }

if (argc != 2) {
            fprintf(stderr,"usage: client hostname\n");
            exit(1);
        }

    for (int i = 0; i < TRIALS; i++)
    {


        if ((he=gethostbyname(argv[1])) == NULL) {  // get the host info
            herror("gethostbyname");
            exit(1);
        }

        if ((sockfd = socket(PF_INET, SOCK_STREAM, 0)) == -1) {
            perror("socket");
            exit(1);
        }

        their_addr.sin_family = AF_INET;    // host byte order
        their_addr.sin_port = htons(PORT);  // short, network byte order
        their_addr.sin_addr = *((struct in_addr *)he->h_addr);
        memset(&(their_addr.sin_zero), '\0', 8);  // zero the rest of the struct

        if (connect(sockfd, (struct sockaddr *)&their_addr,
                                          sizeof(struct sockaddr)) == -1) {
        perror("connect");
        exit(1);
        }

        if ((numbytes=recv(sockfd, buf, MAXDATASIZE-1, 0)) == -1) {
            perror("recv");
            exit(1);
        }
        gettimeofday(&tval_after, NULL); //lets get the current time when message is received


        //printf("Received message: %s\n",buf); //uncomment if want to receive message
        printf("Received %d bytes\n", numbytes);
        printf("Time when message is received = %ld.%06ld\n\n", (long int)tval_after.tv_sec, (long int)tval_after.tv_usec);
        fprintf(f, "%ld.%06ld\n", (long int)tval_after.tv_sec, (long int)tval_after.tv_usec); //storing time into file.
        close(sockfd);
    }

    return 0;
}
