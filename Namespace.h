/**
 *
 * Copyright (c) 2005-2009 Anchorite (TeamX), <anchorite2001@yandex.ru>
 * Copyright (c) 20014-2015 Nirran, phobos2077
 * Copyright (c) 20015 alexeevdv <mail@alexeevdv.ru>
 * Distributed under the GNU GPL v3. For full terms see the file license.txt
 *
 */

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
    int32_t GetSize() const;
    std::string GetStringByIndex(int32_t nIndex) ;
    uint32_t GetOffsetByIndex(int32_t nIndex) ;

    void Dump(CArchive& ar);

public:
    std::string operator [] (uint32_t ulOffset) const;

private:    
    typedef CMap<uint32_t, uint32_t, std::string, const char*> CMapDWordToString;

    CMapDWordToString m_Map;
    CDWordArray m_Order;
};

#endif //NAMESPACE_H
