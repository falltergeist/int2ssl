#include "stdafx.h"
#include "Utility.h"


UINT ReadMSBWord(CArchive& ar, WORD& wValue)
{
	BYTE* pBuffer = reinterpret_cast<BYTE*>(&wValue);
	return (ar.Read(pBuffer + 1, 1) + ar.Read(pBuffer, 1));
}

UINT ReadMSBULong(CArchive& ar, ULONG& ulValue)
{
	BYTE* pBuffer = reinterpret_cast<BYTE*>(&ulValue);
	return (ar.Read(pBuffer + 3, 1) + ar.Read(pBuffer + 2, 1) +
			ar.Read(pBuffer + 1, 1) + ar.Read(pBuffer, 1));
}
