// stdafx.h: Includedatei f�r Standardsystem-Includedateien
// oder h�ufig verwendete projektspezifische Includedateien,
// die nur in unregelm��igen Abst�nden ge�ndert werden.
//

#pragma once

#include "targetver.h"

#include <stdio.h>
#include <tchar.h>

#include <string>
#include <stdint.h>
#include <vector>
#include <memory>
#include <iostream>

#define ASIO_STANDALONE
//#define ASIO_NO_EXCEPTIONS

#pragma warning(push)
#pragma warning(disable: 4592)
#include <asio.hpp>
#pragma warning(pop)

#include <uuid.h>

// Used by the profiler to generate a unique identifier
#define CONCAT(a, b) a ## b
#define UNIQUENAME(prefix) CONCAT(prefix, __LINE__)
