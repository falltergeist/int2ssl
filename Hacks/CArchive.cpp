#include "CArchive.h"

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
    ULONG count;
    count = _file->_istream.readsome(buffer, size);
    //_file->_istream.seekg(_file->_istream.tellg(), std::ios_base::beg);
    return count;
}
