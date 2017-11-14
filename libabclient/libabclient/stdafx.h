// stdafx.h: Includedatei f�r Standardsystem-Includedateien
// oder h�ufig verwendete projektspezifische Includedateien,
// die nur in unregelm��igen Abst�nden ge�ndert werden.
//

#pragma once

#include "targetver.h"

#include <stdio.h>
#include <tchar.h>
#include <cassert>

// Suppress min/max conflicts with STL. For further information visit: http://support.microsoft.com/kb/143208
#ifndef NOMINMAX
#   define NOMINMAX
#endif

#include <sys/timeb.h>
#include <stdint.h>

#define WIN32_LEAN_AND_MEAN
#include "windows.h"

#define ASIO_STANDALONE

#if defined(_DEBUG)
//#define _LOGGING
#else
#undef _LOGGING
#endif

#define AB_UNUSED(P) (void)(P)
