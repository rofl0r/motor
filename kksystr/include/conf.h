#ifndef __KONST_CONF_H__
#define __KONST_CONF_H__

#if defined(__sgi) && !defined(__GNUC__) && (_COMPILER_VERSION >= 721) && defined(_NAMESPACES)
#define __KTOOL_USE_NAMESPACES
#endif

#if defined(__COMO__)
#define __KTOOL_USE_NAMESPACES
#endif

#ifdef __GNUC__
    #ifdef HAVE__G_CONFIG_H
	#include <_G_config.h>
    #endif
#if __GNUC__ < 2 || (__GNUC__ == 2 && __GNUC_MINOR__ < 9)
#else
#define __KTOOL_USE_NAMESPACES
#endif
#endif

#ifdef __KTOOL_USE_NAMESPACES

#define __KTOOL_BEGIN_NAMESPACE		namespace ktool {
#define __KTOOL_END_NAMESPACE		}

using namespace std;

#else

#define __KTOOL_BEGIN_NAMESPACE
#define __KTOOL_END_NAMESPACE

#endif

#define __KTOOL_BEGIN_C			extern "C" {
#define __KTOOL_END_C			}

#endif
