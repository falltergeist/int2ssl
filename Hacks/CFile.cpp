/**
 *
 * Copyright (c) 2005-2009 Anchorite (TeamX), <anchorite2001@yandex.ru>
 * Copyright (c) 20014-2015 Nirran, phobos2077
 * Copyright (c) 20015 alexeevdv <mail@alexeevdv.ru>
 * Distributed under the GNU GPL v3. For full terms see the file license.txt
 *
 */

// C++ standard includes
#include <iostream>

// int2ssl includes
#include "CFile.h"

// Third party includes

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

uint32_t CFile::GetPosition()
{
    switch (_mode)
    {
        case modeWrite:
            return _ostream.tellp();
            break;
    }

    return _istream.tellg();
}

void CFile::Seek(uint32_t position, unsigned int mode)
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

uint32_t CFile::GetLength()
{
    uint32_t size = 0;
    switch (_mode)
    {
        case modeWrite:
        {
            uint32_t oldPosition = _ostream.tellp();
            _ostream.seekp(0, std::ios_base::end);
            size = _ostream.tellp();
            _ostream.seekp(oldPosition, std::ios_base::beg);
            break;
        }
        default:
        {
            uint32_t oldPosition = _istream.tellg();
            _istream.seekg(0, std::ios_base::end);
            size = _istream.tellg();
            _istream.seekg(oldPosition, std::ios_base::beg);
            break;
        }
    }

    return size;
}

