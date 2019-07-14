// stdafx.h: Includedatei für Standardsystem-Includedateien
// oder häufig verwendete projektspezifische Includedateien,
// die nur in unregelmäßigen Abständen geändert werden.
//

#if defined(_MSC_VER)
#pragma once
#endif

#include "targetver.h"

#include <stdio.h>

#include <stdint.h>
#include <cassert>

#include <memory>
#include <string>
#include <vector>
#include <map>
#include <unordered_set>
#include <iostream>

#include <AB/CommonConfig.h>
#include "DebugConfig.h"

#define ASIO_STANDALONE
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable: 4592)
#endif
#include <asio.hpp>
#ifdef _MSC_VER
#pragma warning(pop)
#endif // _MSC_VER

#include <uuid.h>

#include "Utils.h"
#include "Logger.h"

#define WRITE_MINIBUMP
#define PROFILING
