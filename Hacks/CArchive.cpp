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
    _file->_ostream << value;
}

ULONG CArchive::Read(char *buffer, ULONG size)
{
    _file->_istream.read(buffer, size);
    return size;
}
