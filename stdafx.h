#ifndef STDAFX_H
#define STDAFX_H

#include <map>
#include <vector>
#include <fstream>

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
