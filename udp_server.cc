#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/time.h> //for function gettimeofday

#define MYPORT 4950	// the port users will be connecting to

#define MAXBUFLEN 65536 // max # of bytes I will be receiving.

int main(void)
{
	int sockfd;
	struct sockaddr_in my_addr;	// my address information
	struct sockaddr_in their_addr; // connector's address information
	socklen_t addr_len;
	int numbytes;
	char buf[MAXBUFLEN];

	int firsttime, lasttime; //used to calculate first and last time of transmission

	struct timeval tval_before; // for getting current time

    long double duration = 0; // final result of the time
	unsigned long long int totalbytes = 0; // for calculating total data
    unsigned long long int throughput = 0; // will be final result of the throughput (i.e. totalbytes/duration)

    FILE *f = fopen("rcvtimesUDP.txt", "w"); //files which will hold time packets arrived. //make sure this file is in the same directory.
    if (f == NULL)
    {
        printf("Error opening file!\n");
        exit(1);
    }

	if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
		perror("socket");
		exit(1);
	}

	my_addr.sin_family = AF_INET;		 // host byte order
	my_addr.sin_port = htons(MYPORT);	 // short, network byte order
	my_addr.sin_addr.s_addr = INADDR_ANY; // automatically fill with my IP
	memset(&(my_addr.sin_zero), '\0', 8); // zero the rest of the struct

	if (bind(sockfd, (struct sockaddr *)&my_addr,
		sizeof(struct sockaddr)) == -1) {
		perror("bind");
		exit(1);
	}

	addr_len = sizeof(struct sockaddr);

	while(1)
	{
        if ((numbytes = recvfrom(sockfd, buf, MAXBUFLEN , 0,
            (struct sockaddr *)&their_addr, &addr_len)) == -1) {
            perror("recvfrom");
            exit(1);
        }

        if(numbytes == 5) // the packet that triggers the time to start has arrived
        {
            gettimeofday(&tval_before, NULL); // getting current time
            printf("Time when first 'trigger' message is received = %ld.%06ld\n\n", (long int)tval_before.tv_sec, (long int)tval_before.tv_usec);
            firsttime = (int)tval_before.tv_usec;
            break;
        }
	}

    int i = 1; // count number of packets received.
    while(1)
    {
        if ((numbytes = recvfrom(sockfd, buf, MAXBUFLEN , 0,
            (struct sockaddr *)&their_addr, &addr_len)) == -1) {
            perror("recvfrom");
            exit(1);
        }

        printf("Packet #: %d\n", i);
        printf("got packet from %s\n",inet_ntoa(their_addr.sin_addr));
        printf("packet is %d bytes long\n",numbytes);
        gettimeofday(&tval_before, NULL); // getting current time
        printf("Time when message is received = %ld.%06ld\n\n", (long int)tval_before.tv_sec, (long int)tval_before.tv_usec);
        fprintf(f, "%ld.%06ld\n", (long int)tval_before.tv_sec, (long int)tval_before.tv_usec); //storing info into text file
        totalbytes += numbytes;

        if(numbytes == 3) //if last packet lets break out of while loop
        {

            totalbytes -= 3;
            printf("this is the packet that triggers a disconect.\n");
            gettimeofday(&tval_before, NULL); // getting current time
            printf("Current time and time of termination = %ld.%06ld\n\n", (long int)tval_before.tv_sec, (long int)tval_before.tv_usec);
            lasttime = (int)tval_before.tv_usec;
            break;
        }
        //buf[numbytes] = '\0';
        //printf("packet contains \"%s\"\n",buf); //uncomment this line if want to see what packet contains
        ++i;
    }

    printf("A total of %llu bytes were received.\n", totalbytes);
    throughput = totalbytes*1000000/(lasttime-firsttime);//times a million because tv_usec is in microseconds.
    printf("Throughtput of receiving messages is %llu bytes per second\n", throughput);
	close(sockfd);
	return 0;
}


