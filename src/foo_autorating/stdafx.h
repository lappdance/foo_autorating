#ifndef FOO_DAR_INCLUDE_DAR_STDAFX_H
#define FOO_DAR_INCLUDE_DAR_STDAFX_H

#pragma once

/**
 Ignore "universal-character-name encountered in source".
 This warning appears when a Unicode escape sequence is used: \u2034.
**/
#pragma warning(disable : 4428)
/** Build fails if "not all control paths return a value". */
#pragma warning(error : 4715)
/** Build fails if object cannot be destroyed. */
#pragma warning(error : 4527)
/** Build fails if object cannot be created. */
//#pragma warning(error : 4610)
/** Don't warn about unreferenced parameters. */
#pragma warning(disable : 4100)
/** Don't warn if op= cannot be generated. */
#pragma warning(disable : 4512)
/** Don't warn about constant if() statements. */
#pragma warning(disable : 4127)

#ifndef UNICODE
/** Force Unicode */
#define UNICODE 1
#endif //unicode 1

#ifndef _UNICODE
/** Force Unicode */
#define _UNICODE 1
#endif //uniode 2


#ifdef FOO_DAR_EXPORTS
#define FOO_DAR_API __declspec(dllexport)
#else
#define FOO_DAR_API _declspec(dllimport)
#endif //export symbols

#include<string>
#include<sstream>
#include<cassert>
#include<algorithm>

/** Turn off min() / max() macros from Windows headers. */
#define NOMINMAX 1
using std::min;
using std::max;

//show all ATL warnings
#define _ATL_ALL_WARNINGS
//do not append "using namespace xx" to #imported type libraries
//#define _ATL_NO_AUTOMATIC_NAMESPACE
//use new-style dialog handlers
#define _WTL_NEW_PAGE_NOTIFY_HANDLERS 1

//turn off hundreds of warning from ATL
#define _CRT_SECURE_NO_WARNINGS 1
//need this for xp-style common controls
#define ISOLATION_AWARE_ENABLED 1

//foobar SDK requires windows xp
#define WINVER 0x0501
#define _WIN32_WINNT 0x0501
#define _WIN32_IE 0x0600
#define NTDDI_VERSION NTDDI_WINXP

#include<mbstring.h>
#include<foobar/sdk/foobar2000.h>
#include<foobar/ATLHelpers/ATLHelpers.h>
#include<atldlgs.h>
#include<atlddx.h>

namespace auto_rating {
} //~namespace dar

#endif //FOO_DAR_INCLUDE_DAR_STDAFX_H
