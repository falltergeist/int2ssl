#pragma once

#ifndef UTILITY_H
#define UTILITY_H

#include "Hacks/CArchive.h"

// Utility functions
UINT ReadMSBWord(CArchive& ar, WORD& wValue);
UINT ReadMSBULong(CArchive& ar, ULONG& ulValue);

#endif
