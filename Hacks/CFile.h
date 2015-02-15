#ifndef CFILE_H
#define CFILE_H

// C++ standard includes
#include <fstream>

// int2ssl includes
#include "../Hacks/Types.h"

// Third party includes

class CFile
{
    protected:
        unsigned int _mode = 1;


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
        ULONG GetPosition();
        void Seek(ULONG position, unsigned int mode);
        ULONG GetLength();

};

#endif // CFILE_H
