#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/socket.h>
#include<sys/un.h>
#include<signal.h>
#include<unistd.h>
int socket_fd;
const char* const socket_name = "/tmp/socket";
void adieu(int i){
    /** Close the socket */
    close (socket_fd);
    unlink (socket_name);
    exit(0);
}

int main (int argc, char* const argv[]){
    struct sockaddr_un name;
    /** Create the socket. */
    socket_fd = socket (PF_LOCAL, SOCK_STREAM, 0);
    /** Store the server’s name in the socket address. */
    name.sun_family = AF_LOCAL;
    strcpy (name.sun_path, socket_name);
    /** Connect the socket. */
    connect (socket_fd, (struct sockaddr*)&name, SUN_LEN (&name));
    char buff[128];
    /** Write the text on the command line to the socket.*/
    signal(SIGINT,adieu);
    while(1){
        printf("what this time? :\n");
        scanf("%s", buff);
        write (socket_fd, (struct sockaddr*)&buff, sizeof(buff));
    }

    return 0;
}
