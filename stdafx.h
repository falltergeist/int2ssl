#ifndef STDAFX_H
#define STDAFX_H

// C++ standard includes
#include <cstdint>

// int2ssl includes
#include "Hacks/Types.h"
#include "Hacks/CArray.h"

// Third party includes

class CDWordArray : public CArray<uint16_t, uint16_t>
{

};

class UserException
{

};





void AfxThrowUserException();


#endif
