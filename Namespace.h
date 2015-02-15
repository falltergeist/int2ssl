#ifndef NAMESPACE_H
#define NAMESPACE_H

// C++ standard includes

// int2ssl includes
#include "Hacks/CArchive.h"
#include "Hacks/CMap.h"

// Third party includes

class CNamespace
{
public:
    CNamespace();
    virtual ~CNamespace();

public:
    virtual void Serialize(CArchive& ar);

public:
    INT_PTR GetSize() const;
    std::string GetStringByIndex(INT_PTR nIndex) ;
    ULONG GetOffsetByIndex(INT_PTR nIndex) ;

    void Dump(CArchive& ar);

public:
    std::string operator [] (ULONG ulOffset) const;

private:    
    typedef CMap<DWORD, DWORD, std::string, LPCTSTR> CMapDWordToString;

    CMapDWordToString m_Map;
    CDWordArray m_Order;
};

#endif //NAMESPACE_H
