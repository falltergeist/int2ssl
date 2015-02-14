#ifndef STDAFX_H
#define STDAFX_H

#include <map>
#include <vector>
#include <fstream>

#include "Hacks/Types.h"
#include "Hacks/CArray.h"

class CString;


class CDWordArray : public CArray<WORD, WORD>
{

};

class CStringArray : public CArray<CString, CString>
{

};

class UserException
{

};





void AfxThrowUserException();


#endif
