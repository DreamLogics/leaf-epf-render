#ifndef CZLIB_H
#define CZLIB_H

#include <QByteArray>

class CZLib
{
public:

    static bool compress(QByteArray* data, int *crc32);
    static bool decompress(QByteArray* data, int inflatesize, int crc32);

    class ZipCRC
    {
    private:
        static bool crcTableGenerated;
        static quint32 crc32_table[256];

        // Initialized with beginStream(), updated with moreData(), and returned with endStream().
        quint32 streamingCrc;

        quint32 reflect(quint32 ref, char ch);

    public:
        ZipCRC(void);
        virtual ~ZipCRC(void);

        quint32 getCRC(const unsigned char* data, int dataLen, unsigned short *isAscii = 0) const;


        // Streaming CRC.
        void beginStream(void);     // Begin a new CRC calculation.
        void moreData(unsigned char* data, int dataLen);
        quint32 endStream(void);
    };
};

#endif // CZLIB_H
