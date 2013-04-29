#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#define READ_END 0
#define WRITE_END 1

int main (int argc, char **argv){
    if(argc<2){
        printf("\nspecify input file\n");
        exit(1);
    }
    pid_t pid;
    int fd[2];
    int fd2[2];
    pipe(fd);
    pipe(fd2);
    int i;
    pid=fork();
    if (pid==0) {
        close(fd2[WRITE_END]);
        close(fd2[READ_END]);
        close(fd[READ_END]);
        dup2(fd[WRITE_END], STDOUT_FILENO);
        close(fd[WRITE_END]);
        execlp("grep", "grep", "Ala",argv[1], NULL);
    } else {
        pid=fork();
        if (pid==0) {
            close(fd[WRITE_END]);
            dup2(fd[READ_END], STDIN_FILENO);
            close(fd[READ_END]);
            close(fd2[READ_END]);
            dup2(fd2[WRITE_END], STDOUT_FILENO);
            close(fd2[WRITE_END]);
            execlp("grep", "grep","-i","kot",NULL);
        }
        close(fd[WRITE_END]);
        close(fd[READ_END]);
        close(fd2[WRITE_END]);
        dup2(fd2[READ_END], STDIN_FILENO);
        close(fd2[READ_END]);
        execlp("wc", "wc","-l",NULL);
    }
    return 0;
}
