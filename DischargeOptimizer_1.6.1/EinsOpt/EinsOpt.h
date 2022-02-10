#pragma once
#ifndef _LIB_UDE_OPT_H_
#define _LIB_UDE_OPT_H_

#if defined(WIN32) && defined(UDEOPT_BUILD)		// WIN32
#define UDEOPT_EXT_CLASS __declspec(dllexport)
#elif defined(WIN64) && defined(UDEOPT_BUILD)	// WIN64
#define UDEOPT_EXT_CLASS __declspec(dllexport)
#elif defined(WIN64) && defined(_DEBUG)			// WIN64, DEBUG
#pragma comment(lib,"EinsOptd_x64")
#define UDEOPT_EXT_CLASS __declspec(dllimport)
#elif defined(WIN64) && defined(NDEBUG)			// WIN64, RELEASE
#pragma comment(lib,"EinsOpt_x64")
#define UDEOPT_EXT_CLASS __declspec(dllimport)
#elif defined(WIN32) && defined(_DEBUG)			// WIN32, DEBUG
#pragma comment(lib,"EinsOptd_x86")
#define UDEOPT_EXT_CLASS __declspec(dllimport)
#elif defined(WIN32) && defined(NDEBUG)			// WIN32, RELEASE
#pragma comment(lib,"EinsOpt_x86")
#define UDEOPT_EXT_CLASS __declspec(dllimport)
#else
#define UDEOPT_EXT_CLASS
#endif


#pragma warning(disable : 4251) 
#pragma warning(disable : 4996) 
#define _SCL_SECURE_NO_WARNINGS 1

#endif
