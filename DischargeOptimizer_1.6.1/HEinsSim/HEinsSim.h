#pragma once
#ifndef LIB_UDE_H_
#define LIB_UDE_H

#if (defined(_DEBUG) || defined(NDEBUG)) && (!defined(WIN32) && !defined(WIN64))
#define WIN64	// 64bit Windows Project Auto define option.
#endif

#if defined(WIN32) && defined(UDESIM_BUILD)		// WIN32
#define UDESIM_EXT_CLASS __declspec(dllexport)
#elif defined(WIN64) && defined(UDESIM_BUILD)	// WIN64
#define UDESIM_EXT_CLASS __declspec(dllexport)
#elif defined(UDESIM_BUILD)						// LINUX
#define UDESIM_EXT_CLASS __attribute__((__visibility__("default")))
#elif defined(WIN64) && defined(_DEBUG)			// WIN64, DEBUG
#pragma comment(lib,"HEinsSimd_x64")
#define UDESIM_EXT_CLASS __declspec(dllimport)
#elif defined(WIN64) && defined(NDEBUG)			// WIN64, RELEASE
#pragma comment(lib,"HEinsSim_x64")
#define UDESIM_EXT_CLASS __declspec(dllimport)
#elif defined(WIN32) && defined(_DEBUG)			// WIN32, DEBUG
#pragma comment(lib,"HEinsSimd_x86")
#define UDESIM_EXT_CLASS __declspec(dllimport)
#elif defined(WIN32) && defined(NDEBUG)			// WIN32, RELEASE
#pragma comment(lib,"HEinsSim_x86")
#define UDESIM_EXT_CLASS __declspec(dllimport)
#else											// LINUX
#define UDESIM_EXT_CLASS
#endif

#if defined(WIN32) || defined(WIN64)
#pragma warning(disable : 4251)
#pragma warning(disable : 4996)
#define _SCL_SECURE_NO_WARNINGS 1
#endif

#include <list>
#include <vector>
#include <queue>
#include <tuple>
#include <map>

#include <string>
#include <thread>
#include <mutex>

#include <random>
#include <ctime>
#include <algorithm>
#include <utility>

#include <sstream>

#if !defined(WIN32) && !defined(WIN64)
#include <stdarg.h>
#include <string.h>
#include <stdexcept>
#endif

#define ENGINE_PATH			"HEINSSIM_PATH"
#define LICENSE_FILE		"HEinsSimLicense.dat"

#endif
