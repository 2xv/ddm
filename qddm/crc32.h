//  #=============================#
//  # CRC32 - Version ?.?.?       #
//  #                             #
//  # Author: ?                   #
//  # Date:   March 2016 (v?.?.?) #
//  #=============================#

#ifndef CRC32_H
#define CRC32_H

class crc32
{
    static const unsigned int crc32_table [];

public:

    static unsigned int update (unsigned int crc, int buf_size, void *buf);

    static void write (unsigned int crc, void *buf)
    {
        unsigned char *b = static_cast <unsigned char *> (buf);

        b [0] =  crc >> 24;
        b [1] = (crc >> 16) & 0xFF;
        b [2] = (crc >>  8) & 0xFF;
        b [3] =  crc        & 0xFF;
    }
};

#endif    // CRC32_H
