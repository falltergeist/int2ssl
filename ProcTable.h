#pragma once

#ifndef PROC_TABLE_H
#define PROC_TABLE_H

#include "Hacks/CArchive.h"

// CProcDescriptor
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
    ULONG m_ulNameOffset;           // Index into namespace for procedure name
    ULONG m_ulType;                 // Type of function (P_TIMED, P_CONDITIONAL, none)
    ULONG m_ulTime;                 // Time this proc should go off, if timed
    ULONG m_ulExpressionOffset;     // Pointer to conditional code, if conditional
    ULONG m_ulBodyOffset;           // Pointer to procedure
    ULONG m_ulNumArgs;              // Number of args to procedure
};

class CProcTable
{
public:
    CProcTable();
    virtual ~CProcTable();

public:
    virtual void Serialize(CArchive& ar);

    INT_PTR GetSize();
    ULONG GetSizeOfProc(INT_PTR nIndex);
    ULONG GetOffsetOfProcSection();

    void Dump(CArchive& ar);

public:
    CProcDescriptor& operator [] (INT_PTR nIndex);

private:
    // CProcTableArray
    typedef CArray<CProcDescriptor, CProcDescriptor&> CProcTableArray;

    CProcTableArray m_Table;
    CDWordArray m_ProcSize;
    ULONG m_ulOffsetOfProcSection;
};

#endif
