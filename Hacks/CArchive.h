/**
 *
 * Copyright (c) 2005-2009 Anchorite (TeamX), <anchorite2001@yandex.ru>
 * Copyright (c) 2014-2015 Nirran, phobos2077
 * Copyright (c) 2015 alexeevdv <mail@alexeevdv.ru>
 * Distributed under the GNU GPL v3. For full terms see the file license.txt
 *
 */

#ifndef CARCHIVE_H
#define CARCHIVE_H

// C++ standard includes
#include <fstream>

// int2ssl includes
#include "../Hacks/CFile.h"

// Third party includes

class CArchive
{
protected:
    CFile* _file = 0;

public:
    static const unsigned int load = 1;
    static const unsigned int store = 2;

    CArchive(CFile* file, unsigned int mode);
    void Flush();

    CFile* GetFile();

    void WriteString(const char * value);
    void WriteString(std::string value);

    uint32_t Read(char * buffer, uint32_t size);

    std::ofstream* ofstream();

};

#endif // CARCHIVE_H
