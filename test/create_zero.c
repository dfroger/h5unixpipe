#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>

int main() {
    size_t i, n=512;
    char* buffer = malloc(n);
    for (i=0 ; i<n ; i++)
        buffer[i] = '0';

    FILE* f;
    f = fopen("zero512.bin","w");
    fwrite(buffer,1,n,f);
    fclose(f);

    free(buffer);
}
