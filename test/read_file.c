#define HOL_STANDARD_IMPLEMENTATION                     // To put in only one file (main.c for exemple).
                                                        // you can also define HOL_IMPLENTATION for all HOL libraire

#include <HOL/HOL_standard.h>                           // Or any path you choose to put

#include <stdio.h>
#include <string.h>

i32 main(int argc, char** argv)                         // More explicit types (u32, f32, i32 ...)
{
        if(argc < 2)
        {
                fprintf(stderr, ANSI_RED_TEXT("Error ") ": you have to give a file\nexemple : cat file.txt\n");
                return -1;
        }
        HOL_buffer fileContent = HOL_readFile(argv[1]); // File handling function and struct to manage it
        if(fileContent.error != HOLY_SUCCESS)
        {
                if(fileContent.error == HOL_FILE_EMPTY)
                {
                        fprintf(stderr, ANSI_PURPLE_TEXT("Warning ") ": file empty\n");
                        return 0;
                }
                else
                {
                        fprintf(stderr, ANSI_RED_TEXT("Error ") ": cannot access file \n");
                        return -1;
                }
        }

        char* output = malloc(fileContent.size + 1);
        if(output == NULL)
        {
                fprintf(stderr, ANSI_RED_TEXT("Error ") ": no more memory avalable\n");
                return -1;
        }

        memcpy(output, fileContent.buffer, fileContent.size);
        output[fileContent.size] = '\0';

        printf("%s\n", fileContent.buffer);

        free(output);
        return 0;
}
