#ifndef UTILITY_H
#define UTILITY_H

#include "Hacks/CArchive.h"

#include <stdarg.h>


// Utility functions
UINT ReadMSBWord(CArchive& ar, WORD& wValue);
UINT ReadMSBULong(CArchive& ar, ULONG& ulValue);

std::string format(std::string format, ...);
std::string format(std::string format, std::string value);
std::string replace(std::string subject, std::string search, std::string replacement);

#endif
