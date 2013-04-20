#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <time.h>
#include <wait.h>


int licznik = 0;

void fun1(int x) {
    printf(" ->  Odbieram zadany sygnał...\n");
    licznik++;
}

void fun2(int x) {
    printf(" -> W sumie odebrano sygnalow: %d\n", licznik);
    int parent = getppid();
    int l=licznik;
    while(l--){
        printf(" -> Zwracam zadany sygnał...\n");
        kill(parent,SIGUSR1);
        sleep(2);
    }
    printf(" -> Wysyłam potwierdzenie\n");
    kill(parent,SIGUSR2);
    exit(licznik);
}

int main() {
    signal(SIGUSR1,fun1);
    signal(SIGUSR2,fun2);
    while(1){}
    return -100;
}
