#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv) {

        FILE *file = fopen("Robfile", "rb");
        if (file == NULL) {
                printf("Failed to open 'Robfile'");
        }

        char c = fgetc(file);

        long pos = ftell(file);


        printf("%ld %c", pos, c);

        return EXIT_SUCCESS;
}
