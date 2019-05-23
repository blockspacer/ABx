// stdafx.h: Includedatei f�r Standardsystem-Includedateien
// oder h�ufig verwendete projektspezifische Includedateien,
// die nur in unregelm��igen Abst�nden ge�ndert werden.
//

#pragma once

#include "targetver.h"

#include <stdio.h>

#define ASIO_STANDALONE

#define AB_UNUSED(P) (void)(P)

#include <cassert>
#include <AB/CommonConfig.h>
#include "ServiceConfig.h"
#include "Config.h"
#include "DebugConfig.h"

#include <cmath>

// STL
#include <vector>
#include <map>
#include <algorithm>
#include <string>
#include <list>
#include <mutex>
#include <thread>
#include <chrono>
#include <ostream>
#include <iostream>
#include <sstream>
#include <functional>
#include <condition_variable>
#include <unordered_set>

#include <stdint.h>

#define WRITE_MINIBUMP

#include "MathConfig.h"
#include <pugixml.hpp>
#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning(disable: 4592)
#endif
#include <asio.hpp>
#if defined(_MSC_VER)
#pragma warning(pop)
#endif
#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning(disable: 4702 4127)
#endif
#include <kaguya/kaguya.hpp>
#if defined(_MSC_VER)
#pragma warning(pop)
#endif
#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning(disable: 4201 4267 4244)
#endif
#include <ai/SimpleAI.h>
#if defined(_MSC_VER)
#pragma warning(pop)
#endif
#include <uuid.h>

#include "Application.h"

// Used by the profiler to generate a unique identifier
#define CONCAT(a, b) a ## b
#define UNIQUENAME(prefix) CONCAT(prefix, __LINE__)
