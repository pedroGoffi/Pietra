#include <cstdio>
#include <sys/mman.h>

int main(){
    int prot    = PROT_READ | PROT_WRITE;
    int flags   = MAP_SHARED | MAP_ANONYMOUS;
    int fd      = -1;
    int* ptr = (int*)mmap(0, 8, prot, flags, fd, 0);

    

    printf("ptr = %i\n", *ptr);
    printf("int prot    = %zu\n", prot);
    printf("int flags   = %zu\n", flags);
    printf("int fd      = %zu\n", fd);
}  