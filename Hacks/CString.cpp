#include "CString.h"

#include <algorithm>

CString::CString()
{
}

CString::CString(char value)
{
    _string.push_back(value);
}

CString::CString(std::string string)
{
    _string = string;
}

CString::CString(const CString& cstring)
{
    _string = cstring._string;
}

const char * CString::c_str() const
{
    return _string.c_str();
}

CString& CString::operator=(const char* value)
{
    _string += value;
    return *this;
}

CString& CString::operator+=(const char* value)
{
    _string += value;
    return *this;
}

CString& CString::operator+(const char* value)
{
    _string += value;
    return *this;
}

bool CString::operator==(CString& other)
{
    return _string == other._string;
}

CString::operator const char* () const
{
    return _string.c_str();
}

void CString::MakeLower()
{
    std::transform(_string.begin(), _string.end(), _string.begin(), ::tolower);
}

ULONG CString::GetLength()
{
    return _string.length();
}

char* CString::GetBuffer(ULONG size)
{
    return (char*) _string.data();
}


void CString::ReleaseBufferSetLength(ULONG size)
{
    _string.resize(size);
}

void CString::ReleaseBuffer()
{

}

void CString::Format(const char *format, ...)
{
    char buff[1024];
    int size;

    va_list args;
    va_start( args, format );

    size = vsprintf(buff, format, args);
    std::string stroka(buff, size);
    _string = stroka;
}

std::string CString::str()
{
    return _string;
}

void CString::Replace(CString search, CString replace)
{
    std::string str = _string;
    size_t pos = 0;
    while((pos = str.find(search.str(), pos)) != std::string::npos)
    {
        str.replace(pos, search.str().length(), replace.str());
        pos += replace.str().length();
    }
    _string = str;
}
