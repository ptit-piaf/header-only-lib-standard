#define HOL_IMPLEMENTATION
#include <HOL/HOL_standard.h>

#include <stdio.h>
#include <stdlib.h>

int main(u32 argc, char** argv)
{
        if(argc < 2)
                return 1;

        char* dirPath = HOL_getDirPathFromFilePath(argv[1]);
        printf("the directory path of your file is %s\n", dirPath);
        free(dirPath);

        return 0;
}
