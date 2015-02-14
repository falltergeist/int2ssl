#include "CArchive.h"

#include <iostream>

CArchive::CArchive(CFile* file, unsigned int mode)
{
    _file = file;
}

void CArchive::Flush()
{
}

CFile* CArchive::GetFile()
{
    return _file;
}

void CArchive::WriteString(const char * value)
{
    std::cout << _file->_ostream.is_open() << std::endl;
    _file->_ostream << value;
}

ULONG CArchive::Read(char *buffer, ULONG size)
{
    ULONG count;
    count = _file->_istream.readsome(buffer, size);
    return count;
}
