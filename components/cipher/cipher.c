#include "cipher.h"
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

char *cesar_decrypt(const char *input, int shift)
{
    int len = strlen(input);
    char *result = malloc(len + 1);
    if (!result)
        return NULL;

    shift = shift % 26;

    for (int i = 0; i < len; ++i)
    {
        char c = input[i];

        if (c >= 'A' && c <= 'Z')
        {
            c = (c - 'A' - shift + 26) % 26 + 'A';
        }
        else if (c >= 'a' && c <= 'z')
        {
            c = (c - 'a' - shift + 26) % 26 + 'a';
        }
        else if (c >= '0' && c <= '9')
        {
            c = (c - '0' - shift + 10) % 10 + '0';
        }
        else if (c == '$')
        {
            c = ' ';
        }

        result[i] = c;
    }

    result[len] = '\0';
    return result;
}