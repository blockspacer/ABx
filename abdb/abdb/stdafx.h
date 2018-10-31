// stdafx.h: Includedatei f�r Standardsystem-Includedateien
// oder h�ufig verwendete projektspezifische Includedateien,
// die nur in unregelm��igen Abst�nden ge�ndert werden.
//

#pragma once

#include "targetver.h"

#include <stdio.h>

#include <iostream>
#include <map>
#include <string>
#include <memory>

#include "DebugConfig.h"

#define USE_SQLITE
#define USE_MYSQL
#define USE_PGSQL
#ifdef _WIN32
#define USE_ODBC
#endif

// Used by the profiler to generate a unique identifier
#define CONCAT(a, b) a ## b
#define UNIQUENAME(prefix) CONCAT(prefix, __LINE__)
