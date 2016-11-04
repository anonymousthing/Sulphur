#ifdef WINDOWS
#pragma once

//FIXME: Some of these will be required for Input.
//These #defines save us over 20k loc for each .cpp file that would have otherwise included Windows.h
#define WIN32_LEAN_AND_MEAN
#define NOGDICAPMASKS
#define NOVIRTUALKEYCODES
//#define NOWINMESSAGES
//#define NOWINSTYLES
#define NOSYSMETRICS
#define NOMENUS
#define NOICONS
#define NOKEYSTATES
#define NOSYSCOMMANDS
#define NORASTEROPS
#define NOATOM
#define NOCLIPBOARD
#define NOCOLOUR
//#define NOCTLMGR
#define NODRAWTEXT
#define NOGDI //Do not comment -- causes conflicts.
#define NOKERNEL
//#define NOUSER
#define NONLS
#define NOMB
#define NOMEMMGR
#define NOMETAFILE
#define NOMINMAX
//#define NOMSG
#define NOOPENFILE
#define NOSCROLL
#define NOSERVICE
#define NOSOUND
#define NOTEXTMETRIC
#define NOWH
#define NOWINOFFSETS
#define NOCOMM
#define NOKANJI
#define NOHELP
#define NOPROFILER
#define NODEFERWINDOWPOS
#define NOMCX
#define NOIME

#include <Windows.h>

#include "Log.h"

//Found in WindowsHelper.cpp
void __logWindowsError(Log &log, const char *msg, DWORD errorCode);
#define logWindowsError(message)	do { __logWindowsError(log, message, GetLastError());  } while(false)

#define WINAPI_ENSURE_ZERO(apicall, errorMessage) do { if ((apicall) != 0) { logWindowsError(errorMessage); } } while (false)
#define WINAPI_ENSURE_NOT_ZERO(apicall, errorMessage) do { if ((apicall) == 0) { logWindowsError(errorMessage); } } while (false)

#endif
