#pragma once

// Durch Einbeziehen von"SDKDDKVer.h" wird die h�chste verf�gbare Windows-Plattform definiert.

// Wenn Sie die Anwendung f�r eine fr�here Windows-Plattform erstellen m�chten, schlie�en Sie "WinSDKVer.h" ein, und
// legen Sie das _WIN32_WINNT-Makro auf die zu unterst�tzende Plattform fest, bevor Sie "SDKDDKVer.h" einschlie�en.
//Windows 2000	0x0500
//Windows Xp	0x0501
//Windows 2003	0x0502
//Windows Vista	0x0600
//Windows Seven 0x0601
#define _WIN32_WINNT 0x0601

#include <SDKDDKVer.h>
