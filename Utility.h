#ifndef UTILITY_H
#define UTILITY_H

// C++ standard includes
#include <stdarg.h>

// int2ssl includes
#include "Hacks/CArchive.h"

// Third party includes

uint32_t ReadMSBWord(CArchive& ar, uint16_t& wValue);
uint32_t ReadMSBULong(CArchive& ar, uint32_t& ulValue);

std::string format(std::string format, ...);
std::string format(std::string format, std::string value);
std::string replace(std::string subject, std::string search, std::string replacement);

#endif
