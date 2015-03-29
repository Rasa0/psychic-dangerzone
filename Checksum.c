#include "Checksum.h"

#include <stdio.h>

void PrintBits(unsigned char* data, int nBytes) {
    int byte;
    unsigned char bit;

    for (byte = 0; byte < nBytes; ++byte)
    {
        for (bit = 8; bit > 0; --bit)
        {

            if(data[byte] & (1 << (bit-1))) {
                printf("1");
            } else {
                printf("0");
            }
        }
    }

    printf("\n");
}

crc GetChecksum(unsigned char* message, int nBytes)
{
    crc  remainder = 0;	
    int byte;
    unsigned char bit;

    /*
     * Perform modulo-2 division, a byte at a time.
     */
    for (byte = 0; byte < nBytes-1; ++byte)
    {
        /*
         * Bring the next byte into the remainder.
         */
        remainder ^= message[byte];

        /*
         * Perform modulo-2 division, a bit at a time.
         */
        for (bit = 8; bit > 0; --bit)
        {
            /*
             * Try to divide the current data bit.
             */
            if (remainder & TOPBIT)
            {
                remainder = (remainder << 1) ^ 0xD5;/*POLYNOMIAL*/
            }
            else
            {
                remainder = (remainder << 1);
            }
        }
    }

    /*
     * The final remainder is the CRC result.
     */
    return (remainder);

}   

crc CheckChecksum(unsigned char* message, int nBytes)
{
    crc  remainder = 0;
    int byte;
    unsigned char bit;

    /*
     * Perform modulo-2 division, a byte at a time.
     */
    for (byte = 0; byte < nBytes; ++byte)
    {
        /*
         * Bring the next byte into the remainder.
         */
        remainder ^= message[byte];

        /*
         * Perform modulo-2 division, a bit at a time.
         */
        for (bit = 8; bit > 0; --bit)
        {
            /*
             * Try to divide the current data bit.
             */
            if (remainder & TOPBIT)
            {
                remainder = (remainder << 1) ^ 0xD5;/*POLYNOMIAL*/
            }
            else
            {
                remainder = (remainder << 1);
            }
        }
    }

    /*
     * The final remainder is the CRC result.
     */
    return (remainder);
}

