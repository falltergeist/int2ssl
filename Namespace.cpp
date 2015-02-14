// Namespace.cpp : implementation file
//

#include "stdafx.h"
#include "Namespace.h"
#include "Utility.h"

#include "Hacks/CString.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CNamespace

CNamespace::CNamespace()
{
    m_Map.InitHashTable(128);
}

CNamespace::~CNamespace()
{
}

void CNamespace::Serialize(CArchive& ar)
{
    //if (ar.IsStoring()) {
    if (false)
    {
        //ASSERT(FALSE);
    }
    else {
        m_Map.RemoveAll();
        m_Order.RemoveAll();

        ULONG ulLength;

        if (ReadMSBULong(ar, ulLength) != 4)
        {
            printf("Error: Unable read length of namespace\n");
            AfxThrowUserException();
        }

        if (ulLength != 0xFFFFFFFF)
        {
            ULONG ulTotalRead = 0;
            WORD wLengthOfString;
            LPTSTR  lpszNewString;

            while(ulTotalRead < ulLength)
            {
                CString strNewString;

                if (ReadMSBWord(ar, wLengthOfString) != 2) {
                    printf("Error: Unable read length of string\n");
                    AfxThrowUserException();
                };

                if ((wLengthOfString < 2) || (wLengthOfString & 0x0001)) {
                    printf("Error: Invalid length of string\n");
                    AfxThrowUserException();
                }

                lpszNewString = strNewString.GetBuffer(wLengthOfString);

                if (ar.Read(lpszNewString, wLengthOfString) != wLengthOfString) {
                    strNewString.ReleaseBufferSetLength(0);
                    printf("Error: Unable read string in namespace\n");
                    AfxThrowUserException();
                }

                if ((lpszNewString[wLengthOfString - 1] != '\x00') && (lpszNewString[wLengthOfString - 2] != '\x00')) {
                    strNewString.ReleaseBufferSetLength(0);
                    printf("Error: Invalid end of string in namespace\n");
                    AfxThrowUserException();
                }

                std::string tmpString(lpszNewString);
                strNewString = tmpString.c_str();//.ReleaseBuffer();


                // Convert Nongraphic Characters to Escape sequence
                CString strNonGraph("\\\a\b\f\n\r\t\"");
                CString strEscape("\\abfnrt\"");

                for(int i = 0; i < strNonGraph.GetLength(); i++)
                {
                    strNewString.Replace(CString(strNonGraph[i]), CString("\\" + strEscape[i]));
                }

                m_Map.SetAt(ulTotalRead + 6, strNewString);
                m_Order.Add(ulTotalRead + 6);

                ulTotalRead += (2 + wLengthOfString);
            }

            ULONG ulTerminator;

            if (ReadMSBULong(ar, ulTerminator) != 4)
            {
                printf("Error: Unable read terminator of namespace\n");
                AfxThrowUserException();
            }

            if (ulTerminator != 0xFFFFFFFF) {
                printf("Error: Invalid terminator of namespace\n");
                AfxThrowUserException();
            }

//          //For debugging only
//          for(INT_PTR i = 0; i < GetSize(); i++) {
//              printf("Offset: 0x%08X (%9d), string: %s\n", m_Order[i], m_Order[i], GetStringByIndex(i));
//          }
//
//          printf("\n");
        }
    }
}

INT_PTR CNamespace::GetSize() const
{
    return m_Map.GetSize();
}

CString CNamespace::GetStringByIndex(INT_PTR nIndex)
{
    return (this->operator [] (m_Order.at(nIndex)));
}

ULONG CNamespace::GetOffsetByIndex(INT_PTR nIndex)
{
    return m_Order.at(nIndex);
}

void CNamespace::Dump(CArchive& ar)
{
    CString strOutLine;

    if (m_Order.IsEmpty())
    {
        ar.WriteString("Empty\n");
    }
    else
    {
        for(unsigned int i = 0; i < m_Order.GetSize(); i++)
        {
            strOutLine.Format("0x%08X: \"%s\"\n", m_Order.at(i), GetStringByIndex(i).c_str());
            ar.WriteString(strOutLine);
        }

        ar.WriteString("==================\n");
        strOutLine.Format("%d item(s)\n", m_Order.GetSize());
        ar.WriteString(strOutLine);
    }
}

CString CNamespace::operator [] (ULONG ulOffset) const
{
    CString strResult;

    if (!m_Map.Lookup(ulOffset, strResult))
    {
        printf("Error: No string at offset 0x%08x\n", ulOffset);
        AfxThrowUserException();
    }

    return strResult;
}
