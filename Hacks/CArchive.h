#ifndef CARCHIVE_H
#define CARCHIVE_H

#include "../Hacks/Types.h"
#include "../Hacks/CFile.h"

#include <fstream>

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

    ULONG Read(char * buffer, ULONG size);

    std::ofstream* ofstream();

};

#endif // CARCHIVE_H
