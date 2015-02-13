#include "stdafx.h"
#include "Utility.h"


UINT ReadMSBWord(CArchive& ar, WORD& wValue)
{
    char* pBuffer = reinterpret_cast<char*>(&wValue);
	return (ar.Read(pBuffer + 1, 1) + ar.Read(pBuffer, 1));
}

UINT ReadMSBULong(CArchive& ar, ULONG& ulValue)
{
    char* pBuffer = reinterpret_cast<char*>(&ulValue);
	return (ar.Read(pBuffer + 3, 1) + ar.Read(pBuffer + 2, 1) +
			ar.Read(pBuffer + 1, 1) + ar.Read(pBuffer, 1));
}
