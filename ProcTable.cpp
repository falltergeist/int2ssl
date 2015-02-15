/**
 *
 * Copyright (c) 2005-2009 Anchorite (TeamX), <anchorite2001@yandex.ru>
 * Copyright (c) 20014-2015 Nirran, phobos2077
 * Copyright (c) 20015 alexeevdv <mail@alexeevdv.ru>
 * Distributed under the GNU GPL v3. For full terms see the file license.txt
 *
 */

// C++ standard includes
#include <iostream>

// int2ssl includes
#include "stdafx.h"
#include "ProcTable.h"
#include "Utility.h"
#include "ObjectAttributes.h"

// Third party includes

CProcDescriptor::CProcDescriptor() :
    m_ulNameOffset(0),
    m_ulType(0),
    m_ulTime(0),
    m_ulExpressionOffset(0),
    m_ulBodyOffset(0),
    m_ulNumArgs(0)
{
}

CProcDescriptor::CProcDescriptor(const CProcDescriptor& Item) :
    m_ulNameOffset(Item.m_ulNameOffset),
    m_ulType(Item.m_ulType),
    m_ulTime(Item.m_ulTime),
    m_ulExpressionOffset(Item.m_ulExpressionOffset),
    m_ulBodyOffset(Item.m_ulBodyOffset),
    m_ulNumArgs(Item.m_ulNumArgs)
{
}


CProcDescriptor::~CProcDescriptor()
{
}

CProcDescriptor& CProcDescriptor::operator = (const CProcDescriptor& Item)
{
    if (&Item != this)
    {
        m_ulNameOffset = Item.m_ulNameOffset;
        m_ulType = Item.m_ulType;
        m_ulTime = Item.m_ulTime;
        m_ulExpressionOffset = Item.m_ulExpressionOffset;
        m_ulBodyOffset = Item.m_ulBodyOffset;
        m_ulNumArgs = Item.m_ulNumArgs;
    }

    return (*this);
}

void CProcDescriptor::Serialize(CArchive& ar)
{
    uint32_t uiTotalRead;

    uiTotalRead = ReadMSBULong(ar, m_ulNameOffset);
    uiTotalRead += ReadMSBULong(ar, m_ulType);
    uiTotalRead += ReadMSBULong(ar, m_ulTime);
    uiTotalRead += ReadMSBULong(ar, m_ulExpressionOffset);
    uiTotalRead += ReadMSBULong(ar, m_ulBodyOffset);
    uiTotalRead += ReadMSBULong(ar, m_ulNumArgs);

//    printf("=================\n");
//    printf("uiTotalRead          = %u\n", uiTotalRead);
//    printf("m_ulNameOffset       = 0x%08X\n", m_ulNameOffset);
//    printf("m_ulType             = 0x%08X\n", m_ulType);
//    printf("m_ulTime             = 0x%08X\n", m_ulTime);
//    printf("m_ulExpressionOffset = 0x%08X\n", m_ulExpressionOffset);
//    printf("m_ulBodyOffset       = 0x%08X\n", m_ulBodyOffset);
//    printf("m_ulNumArgs          = 0x%08X\n", m_ulNumArgs);


    if (uiTotalRead != (sizeof(uint32_t) * 6))
    {
        printf("Error: Unable read procedure descriptor\n");
        AfxThrowUserException();
    }
}

void CProcDescriptor::Dump(CArchive& ar)
{
    std::string strOutLine;
    std::string strType;

    if (m_ulType != 0x00000000)
    {
        strType = "( ";

        if (m_ulType & P_TIMED)
        {
            strType += "timed ";
        }

        if (m_ulType & P_CONDITIONAL)
        {
            strType += "conditional ";
        }

        if (m_ulType & P_IMPORT)
        {
            strType += "import ";
        }

        if (m_ulType & P_EXPORT) {
            strType += "export ";
        }

        if (m_ulType & P_CRITICAL) {
            strType += "critical ";
        }

        strType += ")";
    }
    else
    {
        strType = "No special types";
    }

    strOutLine = format("Name offset:       0x%08X\n", m_ulNameOffset);
    ar.WriteString(strOutLine);
    strOutLine = format("Type:              0x%08X  // %s\n", m_ulType, strType.c_str());
    ar.WriteString(strOutLine);
    strOutLine = format("Time:              0x%08X  // %d\n", m_ulTime, m_ulTime);
    ar.WriteString(strOutLine);
    strOutLine = format("Expression offset: 0x%08X\n", m_ulExpressionOffset);
    ar.WriteString(strOutLine);
    strOutLine = format("Body offset:       0x%08X\n", m_ulBodyOffset);
    ar.WriteString(strOutLine);
    strOutLine = format("Number of args:    0x%08X  // %d\n", m_ulNumArgs, m_ulNumArgs);
    ar.WriteString(strOutLine);
}


// CProcTable

CProcTable::CProcTable()
{
}

CProcTable::~CProcTable()
{
}

struct ProcBodyOffset {
    uint32_t m_ulProcIndex;
    uint32_t m_ulBodyOffset;
};

int compareProcBodyOffsets(const void* elem0, const void* elem1)
{
    ProcBodyOffset offset0 = *((ProcBodyOffset*)elem0);
    ProcBodyOffset offset1 = *((ProcBodyOffset*)elem1);

    if (offset0.m_ulBodyOffset < offset1.m_ulBodyOffset)
    {
        return -1;
    }
    else if (offset0.m_ulBodyOffset > offset1.m_ulBodyOffset)
    {
        return 1;
    }
    else
    {
        if (offset0.m_ulProcIndex < offset1.m_ulProcIndex)
        {
            return -1;
        }
        else if (offset0.m_ulProcIndex > offset1.m_ulProcIndex)
        {
            return 1;
        }
        else
        {
            return 0;
        }
    }
}

void CProcTable::Serialize(CArchive& ar)
{
    m_Table.RemoveAll();
    m_ProcSize.RemoveAll();

    uint32_t ulSizeOfTable;
    uint32_t ulRead;

    ar.Flush();
    uint32_t ulSizeOfFile = uint32_t(ar.GetFile()->GetLength());

    ulRead = ReadMSBULong(ar, ulSizeOfTable);

    if (ulRead != sizeof(ulSizeOfTable))
    {
        printf("Error: Unable read size of procedures table\n");
        AfxThrowUserException();
    }

    m_Table.SetSize(ulSizeOfTable);
    m_ProcSize.SetSize(ulSizeOfTable);

    uint32_t ulIndexOfProcOffset = 0;
    ProcBodyOffset* pOffsets = new ProcBodyOffset[ulSizeOfTable + 1];

    if (pOffsets == NULL)
    {
//        AfxThrowMemoryException();
        throw 1;
    }

    for(uint32_t i = 0; i < ulSizeOfTable; i++)
    {
//      printf("======== %u =========\n", i);
        m_Table[i].Serialize(ar);
        m_ProcSize[i] = 0;       // Initialize size of procedure

        if (!(m_Table[i].m_ulType & P_IMPORT))
        {
            if ((m_Table[i].m_ulBodyOffset != 0) && (m_Table[i].m_ulBodyOffset != ulSizeOfFile))
            {
                pOffsets[ulIndexOfProcOffset].m_ulProcIndex = i;
                pOffsets[ulIndexOfProcOffset].m_ulBodyOffset = m_Table[i].m_ulBodyOffset;
                ulIndexOfProcOffset++;
            }
            else
            {
                m_Table[i].m_ulType |= P_NOTIMPLEMENTED;
            }
        }
    }

    pOffsets[ulIndexOfProcOffset].m_ulProcIndex = ulIndexOfProcOffset + 1;
    pOffsets[ulIndexOfProcOffset].m_ulBodyOffset = ulSizeOfFile;

    qsort(pOffsets, ulIndexOfProcOffset, sizeof(ProcBodyOffset), compareProcBodyOffsets);

    // Sizes of procedures
    for(uint32_t i = 0; i < ulIndexOfProcOffset; i++)
    {
        if (pOffsets[i + 1].m_ulBodyOffset >=  pOffsets[i].m_ulBodyOffset)
        {
            m_ProcSize[pOffsets[i].m_ulProcIndex] = pOffsets[i + 1].m_ulBodyOffset -  pOffsets[i].m_ulBodyOffset;
        }
        else
        {
            printf("Error: Procedures are not sorted in ascending address order\n");
            AfxThrowUserException();
        }
    }

    delete [] pOffsets;

    for(uint32_t i = 0; i < ulSizeOfTable; i++)
    {
        if (m_Table[i].m_ulBodyOffset != 0)
        {
            m_ulOffsetOfProcSection = m_Table[i].m_ulBodyOffset;
            break;
        }
    }

//    // For debugging only
//    printf("Total: %d\n", m_Table.GetSize());

//    for(i = 0; i < ulSizeOfTable; i++)
//    {
//        printf("Offset: 0x%08X, size: 0x%08X\n", m_Table[i].m_ulBodyOffset, m_ProcSize[i]);
//    }

//    printf("m_ulOffsetOfProcSection: 0x%08X\n", m_ulOffsetOfProcSection);
}

INT_PTR CProcTable::GetSize()
{
    return m_Table.GetSize();
}

uint32_t CProcTable::GetSizeOfProc(INT_PTR nIndex)
{
    if ((nIndex < 0) || (nIndex > m_ProcSize.GetUpperBound()))
    {
        printf("Warning: Invalid index of procedure (%d). Exception will be thrown\n", nIndex);
        AfxThrowUserException();
    }

    return m_ProcSize[nIndex];
}

uint32_t CProcTable::GetOffsetOfProcSection()
{
    return m_ulOffsetOfProcSection;
}


void CProcTable::Dump(CArchive& ar)
{
    std::string strOutLine;

    for(unsigned int i = 0; i < m_Table.GetSize(); i++)
    {
        strOutLine = format("======== Procedure %d ========\n", i);

        ar.WriteString(strOutLine);
        m_Table[i].Dump(ar);
        ar.WriteString("\n");
    }
}


CProcDescriptor& CProcTable::operator [] (INT_PTR nIndex)
{
    if ((nIndex < 0) || (nIndex > m_ProcSize.GetUpperBound()))
    {
        printf("Warning: Invalid index of procedure (%d). Exception will be thrown\n", nIndex);
        AfxThrowUserException();
    }

    return m_Table[nIndex];
}
