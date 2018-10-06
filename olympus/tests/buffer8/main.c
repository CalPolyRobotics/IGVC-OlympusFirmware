
#include <stdio.h>
#include <string.h>

#include "buffer8.h"

#define RUNTEST(x) \
if (test ## x ()) \
{ \
    printf("Test %d failed\n", x); \
}

#define BEGINTEST(x) \
buffer8_t buffer; \
uint8_t data[(x)]; \
memset(data, 0, (x)); \
buffer8_init(&buffer, data, (x));

#define CHECKBUF(x) \
if (buffer8_get(&buffer) != x) \
{ \
    return 1; \
}

#define CHECKBUFSIZE(size, bytes) \
if (buffer8_bytes(&buffer) != (bytes)) \
{ \
    return 1; \
} \
if (buffer8_space(&buffer) != ((size) - (bytes))) \
{ \
    return 1; \
} \
if (buffer8_empty(&buffer) != ((bytes) == 0)) \
{ \
    return 1; \
} \
if (buffer8_full(&buffer) != ((size) == (bytes))) \
{ \
    return 1; \
}

int test1()
{
    BEGINTEST(64)

    buffer8_put(&buffer, 'a');
    buffer8_put(&buffer, 'b');
    buffer8_put(&buffer, 'c');
    buffer8_put(&buffer, 'd');

    CHECKBUFSIZE(64, 4)

    CHECKBUF('a')
    CHECKBUF('b')
    CHECKBUF('c')
    CHECKBUF('d')

    CHECKBUFSIZE(64, 0)

    return 0;
}

int test2()
{
    BEGINTEST(64)

    buffer8_write(&buffer, (uint8_t*)"HELLO", 5);

    CHECKBUFSIZE(64, 5)

    CHECKBUF('H')
    CHECKBUF('E')

    CHECKBUFSIZE(64, 3)

    CHECKBUF('L')
    CHECKBUF('L')
    CHECKBUF('O')

    CHECKBUFSIZE(64, 0)

    return 0;
}

int test3()
{
    char *testStr = "This is too long!";
    BEGINTEST(4)

    buffer8_write(&buffer, (uint8_t*)testStr, strlen(testStr));

    CHECKBUFSIZE(4, 0)

    return 0;
}

int test4()
{
    BEGINTEST(4)

    buffer8_put(&buffer, 'a');
    buffer8_put(&buffer, 'b');
    buffer8_put(&buffer, 'c');
    buffer8_put(&buffer, 'd');

    CHECKBUFSIZE(4, 4)

    buffer8_put(&buffer, '!');

    CHECKBUFSIZE(4, 4)

    buffer8_put(&buffer, '@');

    CHECKBUFSIZE(4, 4)

    CHECKBUF('a')

    CHECKBUFSIZE(4, 3)

    CHECKBUF('b')
    CHECKBUF('c')
    CHECKBUF('d')

    CHECKBUFSIZE(4, 0)

    return 0;
}

int main()
{
    RUNTEST(1)
    RUNTEST(2)
    RUNTEST(3)
    RUNTEST(4)

    return 0;
}

