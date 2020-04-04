#include "utilities.h"
#include <stdlib.h>
#include <stdio.h>

int fetch_data(const char * const filePath, void ** buffer, unsigned int * length, const char * read_format)
{
    if (read_format == NULL)
    {
        return NULL_ARGUMENT;
    }

    FILE* file = fopen(filePath, read_format);

    if (file == NULL)
    {
        return FILE_ERROR;
    }

    fseek(file, 0, SEEK_END);
    *length = ftell(file) + 1;
    fseek(file, 0, SEEK_SET);

    *buffer = (void *)malloc(sizeof(unsigned char) * (*length));

    if (*buffer == NULL)
    {
        return BAD_MEMORY_ALLOC;
    }

    fread(*buffer, sizeof(unsigned char), *length - 1, file);
    *((unsigned char *)*buffer + *length - 1) = NULL;

    fclose(file);
    return STATUS_SUCCESS;
}

int write_data(const char * const filePath, const void * buffer, const unsigned int length)
{
    if (buffer == NULL)
    {
        return NULL_ARGUMENT;
    }

    FILE * file = fopen(filePath, "wb");

    if (file == NULL)
    {
        return FILE_ERROR;
    }

    fwrite(buffer, sizeof(unsigned char), length, file);
    fclose(file);
    return STATUS_SUCCESS;
}

int print_bits(const void * const data, const unsigned int size, const unsigned int format, const bool little_endian)
{
    if (data == NULL)
    {
        return NULL_ARGUMENT;
    }

    if (format > FORMAT_BYTES)
    {
        return INVALID_FORMAT;
    }

    for (unsigned int index = 0; index < size; ++index)
    {
        if (format == FORMAT_BITS)
        {
            if (little_endian)
            {
                for (unsigned char bit = 128; bit > 0; bit >>= 1)
                {
                    printf("%d", (*((const char *)data + index) & bit) != 0);
                }
            }
            else
            {
                for (unsigned char bit = 1; bit != 0; bit <<= 1)
                {
                    printf("%d", (*((const char *)data + index) & bit) != 0);
                }
            }

            printf(" ");
        }
        else if (format == FORMAT_ASCII)
        {
            switch (*((const char *)data + index))
            {
            case '\n':
                printf("`LF`");
                break;
            case '\t':
                printf("`TAB`");
                break;
            case 0:
                printf("`NULL`");
                break;
            case 32:
                printf("`Space`");
                break;
            default:
                printf("%c", *((const char *)data + index));
                break;
            }
        }
        else if (format == FORMAT_BYTES)
        {
            printf("%d", *((const char *)data + index));
        }
    }

    return 0;
}
