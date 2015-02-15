#ifndef UTILITY_H
#define UTILITY_H

// C++ standard includes
#include <stdarg.h>

// int2ssl includes
#include "Hacks/CArchive.h"

// Third party includes

UINT ReadMSBWord(CArchive& ar, WORD& wValue);
UINT ReadMSBULong(CArchive& ar, ULONG& ulValue);

std::string format(std::string format, ...);
std::string format(std::string format, std::string value);
std::string replace(std::string subject, std::string search, std::string replacement);

#endif
