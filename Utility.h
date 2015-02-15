/**
 *
 * Copyright (c) 2005-2009 Anchorite (TeamX), <anchorite2001@yandex.ru>
 * Copyright (c) 20014-2015 Nirran, phobos2077
 * Copyright (c) 20015 alexeevdv <mail@alexeevdv.ru>
 * Distributed under the GNU GPL v3. For full terms see the file license.txt
 *
 */

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
