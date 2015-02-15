/**
 *
 * Copyright (c) 2005-2009 Anchorite (TeamX), <anchorite2001@yandex.ru>
 * Copyright (c) 2014-2015 Nirran, phobos2077
 * Copyright (c) 2015 alexeevdv <mail@alexeevdv.ru>
 * Distributed under the GNU GPL v3. For full terms see the file license.txt
 *
 */

#ifndef CFILE_H
#define CFILE_H

// C++ standard includes
#include <fstream>
#include <stdint.h>

// int2ssl includes

// Third party includes

class CFile
{
    protected:
        unsigned int _mode;


    public:
        std::ifstream _istream;
        std::ofstream _ostream;
        static const unsigned int modeRead = 1;
        static const unsigned int modeWrite = 2;
        static const unsigned int shareDenyWrite = 4;
        static const unsigned int modeCreate = 8;
        static const unsigned int typeText = 16;
        static const unsigned int begin = 32;


        CFile();

        bool Open(std::string name, unsigned int mode);
        uint32_t GetPosition();
        void Seek(uint32_t position, unsigned int mode);
        uint32_t GetLength();

};

#endif // CFILE_H
