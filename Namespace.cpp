/**
 *
 * Copyright (c) 2005-2009 Anchorite (TeamX), <anchorite2001@yandex.ru>
 * Copyright (c) 20014-2015 Nirran, phobos2077
 * Copyright (c) 20015 alexeevdv <mail@alexeevdv.ru>
 * Distributed under the GNU GPL v3. For full terms see the file license.txt
 *
 */

// C++ standard includes

// int2ssl includes
#include "stdafx.h"
#include "Namespace.h"
#include "Utility.h"

// Third party includes

CNamespace::CNamespace()
{
    m_Map.InitHashTable(128);
}

CNamespace::~CNamespace()
{
}

void CNamespace::Serialize(CArchive& ar)
{
    m_Map.RemoveAll();
    m_Order.RemoveAll();

    uint32_t ulLength;

    if (ReadMSBULong(ar, ulLength) != 4)
    {
        printf("Error: Unable read length of namespace\n");
        AfxThrowUserException();
    }

    if (ulLength == 0xFFFFFFFF) return;

    uint32_t ulTotalRead = 0;
    uint16_t wLengthOfString;
    char*  lpszNewString;

    while(ulTotalRead < ulLength)
    {
        std::string strNewString;

        if (ReadMSBWord(ar, wLengthOfString) != 2)
        {
            printf("Error: Unable read length of string\n");
            AfxThrowUserException();
        };

        if ((wLengthOfString < 2) || (wLengthOfString & 0x0001))
        {
            printf("Error: Invalid length of string\n");
            AfxThrowUserException();
        }

        strNewString.resize(wLengthOfString);
        lpszNewString = (char*)strNewString.data();

        if (ar.Read(lpszNewString, wLengthOfString) != wLengthOfString)
        {
            strNewString.resize(0);
            printf("Error: Unable read string in namespace\n");
            AfxThrowUserException();
        }

        if ((lpszNewString[wLengthOfString - 1] != '\x00') && (lpszNewString[wLengthOfString - 2] != '\x00'))
        {
            strNewString.resize(0);
            printf("Error: Invalid end of string in namespace\n");
            AfxThrowUserException();
        }

        std::string tmpString(lpszNewString);
        strNewString = tmpString.c_str();//.ReleaseBuffer();


        // Convert Nongraphic Characters to Escape sequence
        std::string strNonGraph("\\\a\b\f\n\r\t\"");
        std::string strEscape("\\abfnrt\"");

        for(int i = 0; i < strNonGraph.length(); i++)
        {
            strNewString = replace(strNewString, "" + strNonGraph[i], "\\" + strEscape[i]);
        }

        m_Map.SetAt(ulTotalRead + 6, strNewString);
        m_Order.Add(ulTotalRead + 6);

        ulTotalRead += (2 + wLengthOfString);
    }

    uint32_t ulTerminator;

    if (ReadMSBULong(ar, ulTerminator) != 4)
    {
        printf("Error: Unable read terminator of namespace\n");
        AfxThrowUserException();
    }

    if (ulTerminator != 0xFFFFFFFF)
    {
        printf("Error: Invalid terminator of namespace\n");
        AfxThrowUserException();
    }

//          //For debugging only
//          for(int32_t i = 0; i < GetSize(); i++) {
//              printf("Offset: 0x%08X (%9d), string: %s\n", m_Order[i], m_Order[i], GetStringByIndex(i));
//          }
//
//          printf("\n");
}

int32_t CNamespace::GetSize() const
{
    return m_Map.GetSize();
}

std::string CNamespace::GetStringByIndex(int32_t nIndex)
{
    return (this->operator [] (m_Order[nIndex]));
}

uint32_t CNamespace::GetOffsetByIndex(int32_t nIndex)
{
    return m_Order[nIndex];
}

void CNamespace::Dump(CArchive& ar)
{
    std::string strOutLine;

    if (m_Order.IsEmpty())
    {
        ar.WriteString("Empty\n");
    }
    else
    {
        for(unsigned int i = 0; i < m_Order.GetSize(); i++)
        {
            strOutLine = format("0x%08X: \"%s\"\n", m_Order[i], GetStringByIndex(i).c_str());
            ar.WriteString(strOutLine);
        }

        ar.WriteString("==================\n");
        strOutLine = format("%d item(s)\n", m_Order.GetSize());
        ar.WriteString(strOutLine);
    }
}

std::string CNamespace::operator [] (uint32_t ulOffset) const
{
    std::string strResult;

    if (!m_Map.Lookup(ulOffset, strResult))
    {
        printf("Error: No string at offset 0x%08x\n", ulOffset);
        AfxThrowUserException();
    }

    return strResult;
}
