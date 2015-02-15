#include "CFile.h"

#include <iostream>

CFile::CFile()
{
}

bool CFile::Open(std::string name, unsigned int mode)
{
    _mode = (mode & modeRead ? modeRead : modeWrite);

    switch (_mode)
    {
        case modeWrite:
            _ostream.open(name.c_str(), std::fstream::out | std::fstream::trunc);
            return _ostream.is_open();
            break;
        default:
            _istream.open(name.c_str(), std::ios_base::in | std::ios_base::binary);
            return _istream.is_open();
            break;
    }
}

ULONG CFile::GetPosition()
{
    switch (_mode)
    {
        case modeWrite:
            return _ostream.tellp();
            break;
    }

    return _istream.tellg();
}

void CFile::Seek(ULONG position, unsigned int mode)
{
    switch (_mode)
    {
        case modeWrite:
            _ostream.seekp(position, std::ios_base::beg);
            break;
        default:
            _istream.seekg(position, std::ios_base::beg);
            break;
    }
}

ULONG CFile::GetLength()
{
    ULONG size = 0;
    switch (_mode)
    {
        case modeWrite:
        {
            ULONG oldPosition = _ostream.tellp();
            _ostream.seekp(0, std::ios_base::end);
            size = _ostream.tellp();
            _ostream.seekp(oldPosition, std::ios_base::beg);
            break;
        }
        default:
        {
            ULONG oldPosition = _istream.tellg();
            _istream.seekg(0, std::ios_base::end);
            size = _istream.tellg();
            _istream.seekg(oldPosition, std::ios_base::beg);
            break;
        }
    }

    return size;
}

