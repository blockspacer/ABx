// stdafx.h: Includedatei f�r Standardsystem-Includedateien
// oder h�ufig verwendete projektspezifische Includedateien,
// die nur in unregelm��igen Abst�nden ge�ndert werden.
//

#if defined(_MSC_VER)
#pragma once
#endif

#include "targetver.h"

#include <stdio.h>

#include <iostream>
#include <map>
#include <string>
#include <memory>

#include <AB/CommonConfig.h>
#include "DebugConfig.h"

#if !defined(USE_MYSQL) && !defined(USE_PGSQL) && !defined(USE_ODBC) && !defined(USE_SQLITE)
#error "Define at least one database driver"
#endif

// Used by the profiler to generate a unique identifier
#define CONCAT(a, b) a ## b
#define UNIQUENAME(prefix) CONCAT(prefix, __LINE__)
