#include "stdafx.h"
#include "Utility.h"


UINT ReadMSBWord(CArchive& ar, uint16_t& wValue)
{
    char* pBuffer = reinterpret_cast<char*>(&wValue);
    return (ar.Read(pBuffer + 1, 1) + ar.Read(pBuffer, 1));
}

UINT ReadMSBULong(CArchive& ar, ULONG& ulValue)
{
    char* pBuffer = reinterpret_cast<char*>(&ulValue);
    return (ar.Read(pBuffer + 3, 1) + ar.Read(pBuffer + 2, 1) +
            ar.Read(pBuffer + 1, 1) + ar.Read(pBuffer, 1));
}

std::string format(std::string format, ...)
{
    char buffer[1024]; // big enough for any string that will be formated
    int size;

    va_list args;
    va_start( args, format );

    size = vsprintf(buffer, format.c_str(), args);
    return std::string(buffer, size);
}

std::string format(std::string format, std::string value)
{
    char buffer[1024]; // big enough for any string that will be formated
    int size;
    size = sprintf(buffer, format.c_str(), value.c_str());
    return std::string(buffer, size);
}

std::string replace(std::string subject, std::string search, std::string replacement)
{
    size_t pos = 0;
    while((pos = subject.find(search, pos)) != std::string::npos)
    {
        subject.replace(pos, search.length(), replacement);
        pos += replacement.length();
    }
    return subject;
}
