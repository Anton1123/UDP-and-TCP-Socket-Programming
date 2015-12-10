#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include <sys/time.h> //for function gettimeofday

#define MYPORT 3490    // the port users will be connecting to
#define BUFFER 65535
#define BACKLOG 10     // how many pending connections queue will hold

void sigchld_handler(int s)
{
    while(waitpid(-1, NULL, WNOHANG) > 0);
}

int main(void)
{
    int sockfd, new_fd;  // listen on sock_fd, new connection on new_fd
    struct sockaddr_in my_addr;    // my address information
    struct sockaddr_in their_addr; // connector's address information
    socklen_t sin_size;
    struct sigaction sa;
    int yes=1;

    struct timeval tval_before; // for getting current time

    FILE *f = fopen("sendtimesTCP.txt", "w");//files which will hold time packets are sent. //make sure this file is in the same directory.

    if (f == NULL)
    {
        printf("Error opening file!\n");
        exit(1);
    }

    if ((sockfd = socket(PF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket");
        exit(1);
    }

    if (setsockopt(sockfd,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof(int)) == -1) {
        perror("setsockopt");
        exit(1);
    }

    my_addr.sin_family = AF_INET;         // host byte order
    my_addr.sin_port = htons(MYPORT);     // short, network byte order
    my_addr.sin_addr.s_addr = INADDR_ANY; // automatically fill with my IP
    memset(&(my_addr.sin_zero), '\0', 8); // zero the rest of the struct

    if (bind(sockfd, (struct sockaddr *)&my_addr, sizeof(struct sockaddr))
                                                                   == -1) {
        perror("bind");
        exit(1);
    }

    if (listen(sockfd, BACKLOG) == -1) {
        perror("listen");
        exit(1);
    }

    sa.sa_handler = sigchld_handler; // reap all dead processes
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    if (sigaction(SIGCHLD, &sa, NULL) == -1) {
        perror("sigaction");
        exit(1);
    }

    char dummy[BUFFER];
    char message[BUFFER] = "a";

    while(1) {  // main accept() loop
        sin_size = sizeof(struct sockaddr_in);
        if ((new_fd = accept(sockfd, (struct sockaddr *)&their_addr,
                                                       &sin_size)) == -1) {
            perror("accept");
            continue;
        }
        printf("server: got connection from %s\n",
                                           inet_ntoa(their_addr.sin_addr));
        if (!fork()) { // this is the child process
            close(sockfd); // child doesn't need the listener
            if (send(new_fd, message, strlen(message), 0) == -1)
                perror("send");

            gettimeofday(&tval_before, NULL); // getting current time
            printf("Message is %d Bytes long\n", (int)strlen(message));
            printf("Time when message is sent = %ld.%06ld\n\n", (long int)tval_before.tv_sec, (long int)tval_before.tv_usec);
            fprintf(f, "%ld.%06ld\n", (long int)tval_before.tv_sec, (long int)tval_before.tv_usec); //storing info into text file

            close(new_fd);
            exit(0);
        }
        strcpy(dummy, message);
        strcat(message, dummy); //doubling message length
        if(strlen(message) > BUFFER)
        {
            printf("Cannot double message any more. Closing connection...\n");
            break;
        }
        close(new_fd);  // parent doesn't need this
    }

    return 0;
}

