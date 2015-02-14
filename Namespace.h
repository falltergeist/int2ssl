#pragma once

#ifndef NAMESPACE_H
#define NAMESPACE_H

#include "Hacks/CArchive.h"
#include "Hacks/CMap.h"

// CNamespace
class CNamespace : public CObject {
public:
    CNamespace();
    virtual ~CNamespace();

public:
    virtual void Serialize(CArchive& ar);

public:
    INT_PTR GetSize() const;
    CString GetStringByIndex(INT_PTR nIndex) ;
    ULONG GetOffsetByIndex(INT_PTR nIndex) ;

    void Dump(CArchive& ar);

public:
    CString operator [] (ULONG ulOffset) const;

private:
    // CMapDWordToString
    typedef CMap<DWORD, DWORD, CString, LPCTSTR> CMapDWordToString;

    CMapDWordToString m_Map;
    CDWordArray m_Order;
};

#endif
