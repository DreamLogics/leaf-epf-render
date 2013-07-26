#include "czlib.h"
//#include <zlib.h>
#include <QDebug>
#include <QDataStream>
/*
qint32 CZLib::uncompressRaw (unsigned char* dest, quint32* destLen, const unsigned char* source, quint32 sourceLen)
{
    z_stream stream;
    qint32 err;

    stream.next_in = (Bytef*)source;
    stream.avail_in = (uInt)sourceLen;
    // Check for source > 64K on 16-bit machine:
    if ((uInt)stream.avail_in != sourceLen) return Z_BUF_ERROR;

    stream.next_out = dest;
    stream.avail_out = (uInt)*destLen;
    if ((uInt)stream.avail_out != *destLen) return Z_BUF_ERROR;

    stream.zalloc = (alloc_func)0;
    stream.zfree = (free_func)0;

    err = inflateInit2(&stream,-15);
    if (err != Z_OK) return err;

    err = inflate(&stream, Z_FINISH);
    //qDebug() << "after inflate " << err;
    if (err != Z_STREAM_END) {
        inflateEnd(&stream);
        if (err == Z_NEED_DICT || (err == Z_BUF_ERROR && stream.avail_in == 0))
            return Z_DATA_ERROR;
        return err;
    }
    *destLen = stream.total_out;

    err = inflateEnd(&stream);
    return err;
}

qint32 CZLib::compressRaw (unsigned char* dest,quint32* destLen, const unsigned char* source, quint32 sourceLen,qint32 level)
{
    z_stream stream;
    qint32 err;

    stream.next_in = (Bytef*)source;
    stream.avail_in = (uInt)sourceLen;
#ifdef MAXSEG_64K
    // Check for source > 64K on 16-bit machine:
    if ((uInt)stream.avail_in != sourceLen) return Z_BUF_ERROR;
#endif
    stream.next_out = dest;
    stream.avail_out = (uInt)*destLen;
    if ((uInt)stream.avail_out != *destLen) return Z_BUF_ERROR;

    stream.zalloc = (alloc_func)0;
    stream.zfree = (free_func)0;
    stream.opaque = (voidpf)0;

    err = deflateInit2(&stream, level,Z_DEFLATED,-15,8,Z_DEFAULT_STRATEGY);
    if (err != Z_OK) return err;

    err = deflate(&stream, Z_FINISH);
    if (err != Z_STREAM_END) {
        deflateEnd(&stream);
        return err == Z_OK ? Z_BUF_ERROR : err;
    }
    *destLen = stream.total_out;

    err = deflateEnd(&stream);

    return err;
}
*/
bool CZLib::compress(QByteArray* data, qint32 *adler32)
{
    /*ZipCRC crc;

    (*crccc) = crc.getCRC((const unsigned char*)(data->constData()),data->size());*/

    (*data) = qCompress(*data);
    //qDebug() << data->toHex();
    data->remove(0,6);

    QDataStream ds(data->mid(data->size()-4));
    ds >> (*adler32);

    data->remove(data->size()-4,4);

    return true;

    /*quint32 clen,srclen;
    unsigned char* buffer;
    unsigned char* src;

    srclen = data->size();
    src = new unsigned char[srclen];

    memcpy(src,data->constData(),srclen);

    ZipCRC crc;

    (*crccc) = crc.getCRC(src,srclen);

    clen=compressBound(data->size());
    qDebug() << "cbound: " << clen;
    buffer = new unsigned char[clen];
    if (compressRaw(buffer,&clen,src,srclen,Z_DEFAULT_COMPRESSION) != Z_OK)
        return false;

    data->setRawData((char *)(buffer),clen);
    delete buffer;
    delete src;

    return true;*/
}

bool CZLib::decompress(QByteArray* data, int inflatesize, qint32 adler32)
{
    QByteArray size,adler;
    QDataStream ds(&size,QIODevice::WriteOnly);
    ds << inflatesize << (qint16)0x789C;
    QDataStream ads(&adler,QIODevice::WriteOnly);
    ads << adler32;


    data->prepend(size);
    data->append(adler);
    //qDebug() << data->toHex();
    (*data) = qUncompress(*data);

    if (data->size() <= 0)
        return false;

    //do crc check
    //TODO: repair?
    /*ZipCRC crc;
    if (crc.getCRC((const unsigned char*)(data->constData()),data->size()) != crc32)
    {
        qDebug() << "crc error";
        return false;
    }*/

    return true;

    /*qDebug() << "decompress inflsize:" << inflatesize << " data size: "<<data->size();

    quint32 clen,srclen;
    unsigned char* buffer;
    unsigned char* src;
    clen=inflatesize;
    srclen = data->size();
    buffer = new unsigned char[clen];
    src = new unsigned char[srclen];

    memcpy(src,data->constData(),srclen);

    int err = uncompressRaw(buffer,&clen,src,srclen);
    if (err != Z_OK)
    {
        qDebug() << "error: " << err;
        return false;
    }

    data->setRawData((char *)(buffer),clen);

    delete buffer;
    delete src;

    return true;

    //do crc check
    //TODO: repair?
    ZipCRC crc;
    if (crc.getCRC((const unsigned char*)(data->constData()),data->size()) != crc32)
    {
        qDebug() << "crc error";
        return false;
    }
    return true;*/
}

bool CZLib::ZipCRC::crcTableGenerated = false;
quint32 CZLib::ZipCRC::crc32_table[256];


CZLib::ZipCRC::~ZipCRC(void)
{

}

CZLib::ZipCRC::ZipCRC(void)
{
    if (!crcTableGenerated)
    {
        // Call this function only once to initialize the CRC table.
        // This is the official polynomial used by CRC-32
        // in PKZip, WinZip and Ethernet.
        quint32 ulPolynomial = 0x04c11db7;

        // 256 values representing ASCII character codes.
        for(int i = 0; i <= 0xFF; i++)
        {
            crc32_table[i]=reflect(i, 8) << 24;
            for (int j = 0; j < 8; j++)
                crc32_table[i] = (crc32_table[i] << 1) ^ (crc32_table[i] & (1 << 31) ? ulPolynomial : 0);
            crc32_table[i] = reflect(crc32_table[i], 32);
        }

        crcTableGenerated = true;
    }
}

quint32 CZLib::ZipCRC::reflect(quint32 ref, char ch)
{
    quint32 value(0);
    // Swap bit 0 for bit 7
    // bit 1 for bit 6, etc.
    for(int i = 1; i < (ch + 1); i++)
    {
        if(ref & 1)
            value |= 1 << (ch - i);
        ref >>= 1;
    }
    return value;
}

void CZLib::ZipCRC::beginStream(void)
{
    streamingCrc = 0xffffffff;
}

void CZLib::ZipCRC::moreData(unsigned char* data, int dataLen)
{
    if (!data) return;
    if (!dataLen) return;

    while(dataLen--)
    {
        streamingCrc = (streamingCrc >> 8) ^ crc32_table[(streamingCrc & 0xFF) ^ *data++];
    }

    return;
}

quint32 CZLib::ZipCRC::endStream(void)
{
    return streamingCrc ^ 0xffffffff;
}


// Potentially request a guess as to whether this is ascii or binary data.
quint32 CZLib::ZipCRC::getCRC(const unsigned char* buffer, int dataLen, unsigned short *isAscii) const
{
    quint32 ulCRC(0xffffffff);

    if (!isAscii)
    {
    // Perform the algorithm on each character
    // in the string, using the lookup table values.
    while(dataLen--)
        {
        ulCRC = (ulCRC >> 8) ^ crc32_table[(ulCRC & 0xFF) ^ *buffer++];
        }
    }
    else
    {
    unsigned int count[256];
    memset(count,'\0',1024);
    while(dataLen--)
        {
        count[*buffer]++;
        ulCRC = (ulCRC >> 8) ^ crc32_table[(ulCRC & 0xFF) ^ *buffer++];
        }
    int n = 0;
    unsigned ascii_freq = 0;
    unsigned bin_freq = 0;
    while (n < 7)        bin_freq += count[n++];
    while (n < 128)    ascii_freq += count[n++];
    while (n < 256) bin_freq += count[n++];
    *isAscii = (unsigned short)(bin_freq > (ascii_freq >> 2) ? 1 : 0);
    }

    // Exclusive OR the result with the beginning value.
    return ulCRC ^ 0xffffffff;
}
