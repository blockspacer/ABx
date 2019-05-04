// stdafx.h: Includedatei f�r Standardsystem-Includedateien
// oder h�ufig verwendete projektspezifische Includedateien,
// die nur in unregelm��igen Abst�nden ge�ndert werden.
//

#pragma once

#include "targetver.h"
#include <stdint.h>

#define WIN32_LEAN_AND_MEAN             // Selten verwendete Komponenten aus Windows-Headern ausschlie�en

#include <memory>
#include <iostream>

#include <AB/CommonConfig.h>
#include "DebugConfig.h"

#define ASIO_STANDALONE

#if defined(_MSC_VER)
#   pragma warning(push)
#   pragma warning(disable: 4592)
#endif
#include <asio.hpp>
#if defined(_MSC_VER)
#   pragma warning(pop)
#endif

#include <uuid.h>

#define SCHEDULER_MINTICKS 10

// Maximum connections to this server. A single machine can maybe handle up to 3000 concurrent connections.
// https://www.gamedev.net/forums/topic/319003-mmorpg-and-the-ol-udp-vs-tcp/?do=findComment&comment=3052256
#define SERVER_MAX_CONNECTIONS 3000

#define AB_UNUSED(P) (void)(P)
