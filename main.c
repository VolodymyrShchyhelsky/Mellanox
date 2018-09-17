#include <stdio.h>

#include <stdint.h>

#include <stdbool.h>

#include <malloc.h>

#include <stdlib.h>

// TODO: add more headers if needed



/* str2num

* allocates memory for binary representation of long number

* initializes lnum and fill allocated memory by converting

* hexadecimal string representation (nul-terminated str) to

* array of bytes (1 byte store to hex digits from str)

* Returns: positive number of bytes allocated and used

 *          for array of bytes,

*          negative number if memory allocation failed

*          or 0 if str content breaks hexadecimal format

* */

int str2num(const char* str, uint8_t** lnum)

{

    int lnum_len = strlen(str) / 2;
    int index = 0;
    int str_it = 0;

    if(str[0] == '0')
    {
        if(str[1] != 'x')
            return 0;
        else
        {
            lnum_len--;
            str_it = 2;
        }
    }
    if(strlen(str) % 2 != 0)
    {
        lnum_len++;
        index++;
    }

    *lnum = (uint8_t*) malloc(lnum_len);
    if( *lnum == NULL )
        return -1;

    for(; str_it < strlen(str); ++str_it, ++index)
    {
        char c = str[str_it];
        int digit;
        if(c >= '0' && c <= '9')
          digit = (c - '0');
        else if (c >= 'a' && c <= 'f')
          digit = (c - 'a' + 10);
        else if (c >= 'A' && c <= 'F')
          digit = (c - 'A' + 10);
        else return 0;

        if(index % 2 == 0)
            (*lnum)[index / 2] = digit << 4;
        else
            (*lnum)[index / 2] += digit;
    }

    return lnum_len;
}



/* num2str

* allocates memory for string representation of long number

* and convers binary representation to hexadecimal string

 * representation when 1 byte is transformed into 2 characters.

* Parameters: lnum – pointer to beginning of array that represents

*                    long number (length up to 1000 bits)

*             len  - length of array lnum

*             upper – if true hex digits A,B,C,D,E and F are used

*                     if false a, b, c, d, e, f are used instead.

* Returns: pointer to string or NULL if allocation failed.

*/

char * num2str(const uint8_t* lnum, size_t len, bool upper)

{
    int coutOfDigits = len * 2;
    bool firstHalf = 1;
    if(lnum[0] >> 4 == 0)
    {
        coutOfDigits--;
        firstHalf = 0;
    }

    char * ans = (char*) malloc(coutOfDigits * sizeof(char));
    if(ans == NULL)
        return NULL;

    int index = 0;
    for(int i = 0 ; i < coutOfDigits; ++i)
    {
        int value;
        if(firstHalf)
        {
            value = (lnum[index] >> 4);
        }
        else
        {
            value = (lnum[index] & 0xF);
            index++;
        }

        if(value < 10)
            ans[i] = '0' + value;
        else
        {
            if(upper)
                ans[i] = 'A' + value - 10;
            else
                ans[i] = 'a' + value - 10;
        }

        firstHalf = !firstHalf;
    }

    return ans;
}



/* shiftnum

* makes shift of long number stored in lnum (array of size len)

* to pos positions to left or to right (if pos negative)

* Side effect:

* reallocate memory for result storage and change values of len and lnum

* in that case (if new length require change, both increase and decrease)

* Returns: positive number of bytes allocated and used for array of bytes

*          or negative number if memory reallocation failed

*/

int shiftnum(uint8_t** lnum, int *len, int pos)

{
    int AddExcBytes = (abs(pos) + 7) / 8; //Additional bytes for pos > 0 or excessive bytes for pos < 0
    int rightShiftCount;

    if(pos > 0)
    {
        *len += AddExcBytes;
        rightShiftCount = 8 - (pos % 8);
    }
    else
    {
        *len -= AddExcBytes;
        rightShiftCount = abs(pos) % 8;
    }

    *lnum = (uint8_t*) realloc(*lnum, *len);
    if( *lnum == NULL )
        return -1;

    for(int i = *len - 1; i > 0; --i)
    {
        (*lnum)[i] >>= rightShiftCount;
        (*lnum)[i] |= (*lnum)[i-1] << (8 - rightShiftCount);
    }
    (*lnum)[0] >>= rightShiftCount;

    if((*lnum)[0] == 0)
    {
        *len--;
        for(int i = 0; i < *len; ++i)
            (*lnum)[i] = (*lnum)[i + 1];
        *lnum = (uint8_t*) realloc(*lnum, *len);
    }

    return *len;
}



int main(int argc, char* argv[])

{

    if(argc != 3) {

        printf("2 agruments are required:\n");

        printf(" 1) decimal signed number of shifts (to right if <0, or to left if >0)\n");

        printf(" 2) hexadecimal number not longer than 250 digits\n");

        printf("e.g.:\n  %s 3 0xFCB123e00Aa\nresults:\n7E591F00550\n7e591f00550\n", argv[0]);

        return 1;

    }

    int pos = 0;

    if(1 != sscanf(argv[1], "%d", &pos)) {

        printf("ERROR: wrong input format\n");

        printf("First parameter should be decimal number. e.g.: 2 -5 +4 -72 67\n");

        return 2;

    }

    uint8_t * data = NULL;

    printf("%s %s\n", argv[1], argv[2]);

    int size = str2num(argv[2], &data);

    if(0 == size) {

        printf("ERROR: wrong input format\n");

        printf("Supported formats: 1234567890abcdef 0x001abcdef000 FFFF 0xABCDEF");

        return 2;

    }

    if(size < 0) {

        printf("ERROR: internal error\n");

        return 3;

    }

    // TODO: for lines below:

    // 1) add checks to prevent usage of non-allocated memory (e.g. NULL pointers)

    // 2) check value returned by shiftnum and add reaction on error

    // 3) free all memory allocated in heap before `return 0;`

    if(data == NULL)
        printf("Data is NULL\n");

    shiftnum(&data, &size, pos);

    if(data == NULL)
        printf("Data after shiftnum function is NULL\n");

    printf("results:\n");

    printf("%s\n", num2str(data, size, true));

    printf("%s\n", num2str(data, size, false));

    free(data);

    return 0;

}
