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
    uint32_t GetOffsetByIndex(INT_PTR nIndex) ;

    void Dump(CArchive& ar);

public:
    std::string operator [] (uint32_t ulOffset) const;

private:    
    typedef CMap<uint32_t, uint32_t, std::string, const char*> CMapDWordToString;

    CMapDWordToString m_Map;
    CDWordArray m_Order;
};

#endif //NAMESPACE_H
