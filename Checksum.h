#ifndef CHECKSUM_H
#define CHECKSUM_H

typedef unsigned char crc;

/*
 * The width of the CRC calculation and result.
 * Modify the typedef for a 16 or 32-bit CRC standard.
 */
#define WIDTH  (8 * sizeof(crc))
#define TOPBIT (1 << (WIDTH - 1))

crc GetChecksum(unsigned char* message, int nBytes);
crc CheckChecksum(unsigned char* message, int nBytes);

void PrintBits(unsigned char* data, int nBytes);

#endif

