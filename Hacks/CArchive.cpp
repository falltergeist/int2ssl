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
#include "CArchive.h"

// Third party includes

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
