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

void CArchive::WriteString(std::string value)
{
    _file->_ostream << value;
}

uint32_t CArchive::Read(char *buffer, uint32_t size)
{
    _file->_istream.read(buffer, size);
    return size;
}

std::ofstream* CArchive::ofstream()
{
    return &_file->_ostream;
}
