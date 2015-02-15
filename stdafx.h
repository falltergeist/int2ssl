#ifndef STDAFX_H
#define STDAFX_H


#include "Hacks/Types.h"
#include "Hacks/CArray.h"

class CDWordArray : public CArray<WORD, WORD>
{

};

class UserException
{

};





void AfxThrowUserException();


#endif
