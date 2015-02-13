#ifndef CSTRING_H
#define CSTRING_H

#include <string>
#include <stdarg.h>

#include "../Hacks/Types.h"

class CString
{
protected:
    std::string _string;
public:
    CString();
    CString(char value);
    CString(std::string string);
    CString(const CString& cstring);

    const char* c_str() const;
    CString& operator=(const char* value);
    bool operator==(CString& other);
    CString& operator+=(const char* value);
    CString& operator+(const char* value);
    operator const char* () const;
    void MakeLower();
    ULONG GetLength();
    char* GetBuffer(ULONG size);
    void ReleaseBufferSetLength(ULONG size);
    void ReleaseBuffer();
    std::string str();

    void Format(const char * format, ...);
    void Replace(CString search, CString replacement);
};
#endif // CSTRING_H
