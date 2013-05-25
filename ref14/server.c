#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/socket.h>
#include<sys/un.h>
#include<unistd.h>
#include <signal.h>
#include<errno.h>
int socket_fd;
const char* const socket_name ="/tmp/socket";
int client_socket_fd;

void adieu(int i){
    /** Close our end of the connection. */
    close (client_socket_fd);
    /** Remove the socket file.*/
    close (socket_fd);
    unlink (socket_name);
    exit(0);
}

int main (int argc, char* const argv[]){
    int ret;
    struct sockaddr_un name;
    /** Create the socket. */
    socket_fd = socket (PF_LOCAL, SOCK_STREAM, 0);
    /** Indicate that this is a server. */
    name.sun_family = AF_LOCAL;
    strcpy (name.sun_path, socket_name);
    bind (socket_fd, (struct sockaddr*)&name, SUN_LEN (&name));
    /** Listen for connections. */
    listen (socket_fd, 5);
    struct sockaddr_un client_name;
    socklen_t client_name_len;
    /** Accept a connection. */
    client_socket_fd = accept (socket_fd, (struct sockaddr*)&client_name, &client_name_len);
    char buff[128];
    signal(SIGINT,adieu);
    while(1){
        ret = read (client_socket_fd, buff, sizeof(buff));
        if(ret==0)break;
        printf("folk says : %s\n", buff);
    }
    adieu(0);
    return 0;
}
