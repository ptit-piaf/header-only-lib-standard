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

typedef enum
{
        HOLY_SUCCESS = 0,
        HOL_FILE_EMPTY,
        HOL_ALLOC_ERROR,
        HOL_AUTORIZATION_REQUIRED,
        HOL_CREATION_FAILED,
        HOL_NULL_FILE_PATH,
        HOL_FILE_NOT_FIND,
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

#endif

#if defined(HOL_STANDARD_IMPLEMENTATION) || defined(HOL_IMPLEMENTATION)

#include <stdio.h>
#include <stdlib.h>

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

extern void HOL_closeFile(HOL_buffer file);

#endif

#endif
