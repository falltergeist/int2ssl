#ifndef CFILE_H
#define CFILE_H

#include <fstream>

#include "../Hacks/Types.h"
#include "../Hacks/CString.h"

class CFile
{
    protected:
        unsigned int _mode = 1;


    public:
        std::ifstream _istream;
        std::ofstream _ostream;
        static const unsigned int modeRead = 1;
        static const unsigned int modeWrite = 2;
        static const unsigned int shareDenyWrite = 3;
        static const unsigned int modeCreate = 4;
        static const unsigned int typeText = 5;
        static const unsigned int begin = 6;


        CFile();

        bool Open(CString name, unsigned int mode);
        ULONG GetPosition();
        void Seek(ULONG position, unsigned int mode);
        ULONG GetLength();

};

class CStdioFile : public CFile
{
protected:
    unsigned int _mode = 2;
public:
    CStdioFile();
};

#endif // CFILE_H
