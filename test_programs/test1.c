#include <stdio.h>

int dowork ()
{
    char buffer[16];
    int i;
    
    for (i = 0; i < 10; i++) {
        sprintf(buffer, "%d", i);
        printf("%s\n", buffer);
    }
    
    return 1;
}

int main (int argc, char * argv[])
{
    return dowork();
}
