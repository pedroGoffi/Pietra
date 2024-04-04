
#include <netinet/in.h>
#include <sys/socket.h>
#include <iostream>
int main(){
    sockaddr_in sa;
    printf("sizeof(sa.sin_family) =  %li\n", sizeof(sa.sin_family));
    printf("sizeof(sa.sin_port) =  %li\n", sizeof(sa.sin_port));
    printf("sizeof(sa.sin_addr) =  %li\n", sizeof(sa.sin_addr));    
    printf("sizeof(sa.sin_zero) =  %li\n", sizeof(sa.sin_zero));
    
    
}