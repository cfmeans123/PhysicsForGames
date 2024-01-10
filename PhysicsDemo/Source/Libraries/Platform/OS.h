#pragma once 

#include "PlatformCore.h"

#if JM_ON_WINDOWS
#define NOATOM


#define NOGDICAPMASKS
#define NOMETAFILE
#define NOMINMAX

#define NOOPENFILE
#define NORASTEROPS
#define NOSCROLL
#define NOSOUND

#define NOTEXTMETRIC
#define NOWH
#define NOCOMM
#define NOKANJI
#define NOCRYPT

#define NOMINMAX

//#define NOGDI
//#define NOMSG
//#define NOMCX
//#define NOSYSMETRICS

#define WIN32_LEAN_AND_MEAN
#define _CRT_SECURE_NO_WARNINGS
#include <Windows.h>

#endif