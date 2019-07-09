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

#include <AB/CommonConfig.h>
#include "DebugConfig.h"

#if !defined(ASIO_STANDALONE)
#define ASIO_STANDALONE
#endif

#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning(disable: 4592)
#endif
#include <asio.hpp>
#if defined(_MSC_VER)
#pragma warning(pop)
#endif

#include <uuid.h>

#define WRITE_MINIBUMP
#define _PROFILING

#include "Logger.h"

#define MAX_DATA_SIZE (1024 * 1024)
#define MAX_KEY_SIZE 256

#if !defined(USE_MYSQL) && !defined(USE_PGSQL) && !defined(USE_ODBC) && !defined(USE_SQLITE)
#error "Define at least one database driver"
#endif

// Used by the profiler to generate a unique identifier
#define CONCAT(a, b) a ## b
#define UNIQUENAME(prefix) CONCAT(prefix, __LINE__)
