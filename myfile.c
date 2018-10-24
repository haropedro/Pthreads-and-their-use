#include <stdio.h>
#include <unistd.h>
int main(int argc, char const *argv[])
{
    int count  = 10;
    for(int i = 0; i < count; i++)
    {
        fork();
    }
    

    printf("%d\n", getpid());
    return 0;
}

