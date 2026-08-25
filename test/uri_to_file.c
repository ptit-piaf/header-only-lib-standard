#include <stdio.h>

#define HOL_IMPLEMENTATION
#include <HOL/HOL_standard.h>

int main(u32 argc, char** argv)
{
        if(argc <= 1)
        {
                printf("Argument missing\n");
                return 0;
        }

        char filePath[512];
        if(HOL_getUnixFilePathFromUri(argv[1], filePath, 512) != HOLY_SUCCESS)
        {
                printf("Cannot parse uri\n");
                return 1;
        }

        printf("Your file path is \"%s\"\n", filePath);

        return 0;
}
