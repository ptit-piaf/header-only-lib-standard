#ifndef HOL_STANDARD_H
#define HOL_STANDARD_H

#include <stdint.h>

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

typedef float f32;
typedef double f64;

#define MAX(x, y) (x > y : x ? y)
#define MIN(x, y) (x < y : x ? y)
#define REPEAT(x, n) for(u32 _=0;x, _<n; _++);

#define ADD_VEC3(x, y) ((vec3){x[0]+y[0], x[1]+y[1], x[2]+y[2]})
#define MUL_VEC3(x, y) ((vec3){x[0]*y[0], x[1]*y[1], x[2]*y[2]})

#define X 0
#define Y 1
#define Z 2

#define ADVANCEMENT_NULL        0
#define ADVANCEMENT_COMPLETE    0xFFFFFFFF

typedef enum
{
        HOLY_SUCCESS = 0,
        HOL_FILE_EMPTY,
        HOL_ALLOC_ERROR,
        HOL_AUTORIZATION_REQUIRED,
        HOL_CREATION_FAILED,
        HOL_NULL_FILE_PATH,
        HOL_FILE_NOT_FIND,
        HOL_NOT_SUPPORTED,
        HOL_NOT_VALID,
        HOL_INVALID_ARGUMENT,
        HOL_PARSING_FAILED,
        HOL_ERROR_MAX = 0XFFFFFFFF  // 32 bits enum (use for error wich are not already define)
} E_error;

typedef struct
{
        void* buffer;
        u64 size;
        E_error error;
} HOL_buffer;

#define ANSI_RED_TEXT(x) "\e[0;31m" x "\e[0m"
#define ANSI_GREEN_TEXT(x) "\e[0;32m" x "\e[0m"
#define ANSI_YELLOW_TEXT(x) "\e[0;33m" x "\e[0m"
#define ANSI_BLUE_TEXT(x) "\e[0;34m" x "\e[0m"
#define ANSI_PURPLE_TEXT(x) "\e[0;35m" x "\e[0m"
#define ANSI_CYAN_TEXT(x) "\e[0;36m" x "\e[0m"

extern HOL_buffer HOL_readFile(const char* filePath);
extern void HOL_closeFile(HOL_buffer file);
extern char* HOL_getDirPathFromFilePath(const char* filePath);
extern E_error HOL_getUnixFilePathFromUri(const char* uri, char* filePathDst, u32 filePathDstLenght);

#endif

#if defined(HOL_STANDARD_IMPLEMENTATION) || defined(HOL_IMPLEMENTATION)
#ifndef HOL_STANDARD_C
#define HOL_STANDARD_C

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef __unix__ // no implementation for window for now

#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

HOL_buffer HOL_readFile(const char* filePath)
{
        HOL_buffer buffer = {0};
        int fd = open(filePath, O_RDONLY);

        if(fd < 0)
        {
                switch(fd)
                {
                        case EACCES:
                                buffer.error = HOL_AUTORIZATION_REQUIRED;
                        default:
                                buffer.error = HOL_ERROR_MAX;
                }
                return buffer;
        }

        buffer.size = lseek(fd, 0, SEEK_END);
        lseek(fd, 0, SEEK_SET);
        if(buffer.size == 0)
        {
                buffer.error = HOL_FILE_EMPTY;
                return buffer;
        }

        buffer.buffer = malloc(buffer.size);
        if(buffer.buffer == NULL)
        {
                buffer.error = HOL_ALLOC_ERROR;
                return buffer;
        }
        read(fd, buffer.buffer, buffer.size);

        close(fd);
        return buffer;
}

char* HOL_getDirPathFromFilePath(const char* filePath)
{
        u32 dirPathSize = 0;

        u32 filePathSize = strlen(filePath) + 1;
        for(u32 i=0; i<filePathSize; i++)
        {
                switch(filePath[i])
                {
                        case '/':
                                dirPathSize = i;
                                break;
                        case '\0':
                                goto GO_QUIT_LOOP;
                }
        }

GO_QUIT_LOOP:

        if(!dirPathSize)
                return NULL;

        char* dirPath = malloc(dirPathSize+2);
        memcpy(dirPath, filePath, dirPathSize+1);
        dirPath[dirPathSize] = '\0';

        return memcpy(dirPath, filePath, dirPathSize+1);
}

void HOL_closeFile(HOL_buffer file)
{
        free(file.buffer);
}

extern E_error HOL_getUnixFilePathFromUri(const char* uri, char* filePathDst, u32 filePathDstLenght)
{
        u32 uriOffset = 0;

        if(strlen(uri)+1 < 1 || strlen(uri)+1 != strlen(uri)+1 || filePathDstLenght<=1)
                return HOL_INVALID_ARGUMENT;

        if(strlen(uri)+1 >= sizeof("file:/"))
        {
                if(strncmp(uri, "file:/", sizeof("file:/")-1))
                        goto GO_FOR_LOOP;

                if(strlen(uri)+1 == sizeof("file:/"))
                        return HOL_PARSING_FAILED;

                uriOffset = sizeof("file:/")-2;

                if(uri[uriOffset+1] != '/')
                        goto GO_FOR_LOOP;

                if(strlen(uri)+1 == sizeof("file://"))
                        return HOL_PARSING_FAILED;

                uriOffset++;
                if(uri[sizeof("file://")-1] == '/')
                {
                        if(strlen(uri)+1 == sizeof("file:///"))
                                return HOL_PARSING_FAILED;
                        uriOffset++;
                        goto GO_FOR_LOOP;
                }

                uriOffset++;
                for(; uri[uriOffset] != '/'; uriOffset++)
                        if(uri[uriOffset] == '\0')
                                return HOL_PARSING_FAILED;

                if(strlen(uri) == uriOffset+1)
                        return HOL_PARSING_FAILED;

                if(uriOffset == strlen(uri))
                        return HOL_PARSING_FAILED;
        }

GO_FOR_LOOP:

        for(u32 i=0;  i<filePathDstLenght && uri[uriOffset]!='\0';  uriOffset++, i++)
        {
                if(uri[uriOffset] == '%')
                {
                        if(uri[uriOffset+1] == 'a' ||
                           uri[uriOffset+1] == 'b' ||
                           uri[uriOffset+1] == 'c' ||
                           uri[uriOffset+1] == 'd' ||
                           uri[uriOffset+1] == 'e' ||
                           uri[uriOffset+1] == 'f')
                        {
                                filePathDst[i] = (uri[uriOffset+1]-'a'+10) * 16;
                        }
                        else if(uri[uriOffset+1] == 'A' ||
                                uri[uriOffset+1] == 'B' ||
                                uri[uriOffset+1] == 'C' ||
                                uri[uriOffset+1] == 'D' ||
                                uri[uriOffset+1] == 'E' ||
                                uri[uriOffset+1] == 'F')
                        {
                                filePathDst[i] = (uri[uriOffset+1]-'A'+10) * 16;
                        }
                        else if(uri[uriOffset+1] == '0' ||
                                uri[uriOffset+1] == '1' ||
                                uri[uriOffset+1] == '2' ||
                                uri[uriOffset+1] == '3' ||
                                uri[uriOffset+1] == '4' ||
                                uri[uriOffset+1] == '5' ||
                                uri[uriOffset+1] == '6' ||
                                uri[uriOffset+1] == '7' ||
                                uri[uriOffset+1] == '8' ||
                                uri[uriOffset+1] == '9')
                        {
                                filePathDst[i] = (uri[uriOffset+1]-'0') * 16;
                        }
                        else
                                return HOL_PARSING_FAILED;

                        if(uri[uriOffset+2] == 'a' ||
                           uri[uriOffset+2] == 'b' ||
                           uri[uriOffset+2] == 'c' ||
                           uri[uriOffset+2] == 'd' ||
                           uri[uriOffset+2] == 'e' ||
                           uri[uriOffset+2] == 'f')
                        {
                                filePathDst[i] += uri[uriOffset+2]-'a'+10;
                        }
                        else if(uri[uriOffset+2] == 'A' ||
                                uri[uriOffset+2] == 'B' ||
                                uri[uriOffset+2] == 'C' ||
                                uri[uriOffset+2] == 'D' ||
                                uri[uriOffset+2] == 'E' ||
                                uri[uriOffset+2] == 'F')
                        {
                                filePathDst[i] += uri[uriOffset+2]-'A'+10;
                        }
                        else if(uri[uriOffset+2] == '0' ||
                                uri[uriOffset+2] == '1' ||
                                uri[uriOffset+2] == '2' ||
                                uri[uriOffset+2] == '3' ||
                                uri[uriOffset+2] == '4' ||
                                uri[uriOffset+2] == '5' ||
                                uri[uriOffset+2] == '6' ||
                                uri[uriOffset+2] == '7' ||
                                uri[uriOffset+2] == '8' ||
                                uri[uriOffset+2] == '9')
                        {
                                filePathDst[i] += uri[uriOffset+2]-'0';
                        }
                        else
                                return HOL_PARSING_FAILED;

                        uriOffset += 2;
                        continue;
                }

                filePathDst[i] = uri[uriOffset];
        }
}

#endif

#endif

#endif
