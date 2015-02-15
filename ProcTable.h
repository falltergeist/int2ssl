/**
 *
 * Copyright (c) 2005-2009 Anchorite (TeamX), <anchorite2001@yandex.ru>
 * Copyright (c) 20014-2015 Nirran, phobos2077
 * Copyright (c) 20015 alexeevdv <mail@alexeevdv.ru>
 * Distributed under the GNU GPL v3. For full terms see the file license.txt
 *
 */

#ifndef PROC_TABLE_H
#define PROC_TABLE_H

// C++ standard includes

// int2ssl includes
#include "Hacks/CArchive.h"

// Third party includes

class CProcDescriptor
{
public:
    CProcDescriptor();
    CProcDescriptor(const CProcDescriptor& Item);
    virtual ~CProcDescriptor();

public:
    virtual void Serialize(CArchive& ar);

    void Dump(CArchive& ar);

public:
    CProcDescriptor& operator = (const CProcDescriptor& Item);

public:
    uint32_t m_ulNameOffset;           // Index into namespace for procedure name
    uint32_t m_ulType;                 // Type of function (P_TIMED, P_CONDITIONAL, none)
    uint32_t m_ulTime;                 // Time this proc should go off, if timed
    uint32_t m_ulExpressionOffset;     // Pointer to conditional code, if conditional
    uint32_t m_ulBodyOffset;           // Pointer to procedure
    uint32_t m_ulNumArgs;              // Number of args to procedure
};

class CProcTable
{
public:
    CProcTable();
    virtual ~CProcTable();

public:
    virtual void Serialize(CArchive& ar);

    int32_t GetSize();
    uint32_t GetSizeOfProc(int32_t nIndex);
    uint32_t GetOffsetOfProcSection();

    void Dump(CArchive& ar);

public:
    CProcDescriptor& operator [] (int32_t nIndex);

private:
    // CProcTableArray
    typedef CArray<CProcDescriptor, CProcDescriptor&> CProcTableArray;

    CProcTableArray m_Table;
    CDWordArray m_ProcSize;
    uint32_t m_ulOffsetOfProcSection;
};

#endif //PROC_TABLE_H
