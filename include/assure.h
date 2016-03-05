/***************************************************************************
 *   Copyright (C) 2006 by Michael Ambrus <ambrmi09@gmail.com>             *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************
 *   This file originates from the TinKer project ((C) 2006):              *
 *   https://github.com/mambrus/tinker                                     *
 ***************************************************************************/

/***************************************************************************
   Static inline functions. Need to be outside normal #ifndef file_h trap
 ***************************************************************************/
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#include <log.h>
#include "config.h"

extern log_level log_ASSURE;
extern log_level log_ASSERT;

static inline log_level get_loglevel_assert(void)
{
    return log_ASSERT;
}

static inline log_level get_loglevel_assure(void)
{
    return log_ASSURE;
}

static inline void set_loglevel_assert(log_level newlevel)
{
    log_ASSERT = newlevel;
}

static inline void set_loglevel_assure(log_level newlevel)
{
    log_ASSURE = newlevel;
}

#ifdef ENABLE_LOGGING

#define LOGX( WHY, ...)  {              \
    log_level log_tmp;                  \
    switch (WHY) {                      \
        case _ASSERT:                   \
            log_tmp = log_ASSERT;       \
            break;                      \
        case _ASSURE:                   \
            log_tmp = log_ASSURE;       \
            break;                      \
        default:                        \
            log_tmp = log_ASSURE;       \
    }                                   \
                                        \
    switch (log_tmp) {                  \
        case LOG_LEVEL_VERBOSE:         \
            LOGV(__VA_ARGS__);          \
            break;                      \
        case LOG_LEVEL_DEBUG:           \
            LOGD(__VA_ARGS__);          \
            break;                      \
        case LOG_LEVEL_INFO:            \
            LOGI(__VA_ARGS__);          \
            break;                      \
        case LOG_LEVEL_WARNING:         \
            LOGW(__VA_ARGS__);          \
            break;                      \
        case LOG_LEVEL_ERROR:           \
            LOGE(__VA_ARGS__);          \
            break;                      \
        default:                        \
            LOGE(__VA_ARGS__);          \
            break;                      \
    }                                   \
}

#endif
typedef enum { _ASSERT, _ASSURE } assrt_t;

static inline void notify_failure(assrt_t etype, char *sassure,
                                  const char *sfun, char *sfile, int iline)
{
#ifdef ENABLE_LOGGING
    char *why;

    switch (etype) {
        case _ASSERT:
            why = "ASSERT: ";
            break;
        case _ASSURE:
            why = "ASSURE: ";
            break;
        default:
            why = "UNKNOWN: ";
    };

#  ifdef LOG_INCLUDE_FILE_INFO
    if (sfun != NULL) {
        LOGX(etype, "%s %s failed in [%s]\n", why, sassure, sfun);
    } else {
        LOGX(etype, "%s %s failed\n", why, sassure);
    }
#  else
    if (sfun != NULL) {
        LOGX(etype, "%s %s failed in [%s] @ [%s:%d]\n", why, sassure, sfun,
             sfile, iline);
    } else {
        LOGX(etype, "%s %s failed @ [%s:%d]\n", why, sassure, sfile, iline);
    }
#  endif
#else
    if (sfun != NULL)
        fprintf(stderr, "ERROR: %s failed in [%s] @ [%s:%d]\n", sassure,
                sfun, sfile, iline);
    else
        fprintf(stderr, "ERROR: %s failed @ [%s:%d]\n", sassure, sfile, iline);
    fflush(stderr);
#endif
}

static inline void notify_andfail(assrt_t etype, char *sassure,
                                  const char *sfun, char *sfile, int iline)
{
    notify_failure(etype, sassure, sfun, sfile, iline);

#ifndef NDEBUG
    /* Generate coredump */
    fprintf(stderr, "Calling abort() for coredump \n");
    fflush(stderr);
    abort();
    fprintf(stderr, "Abort failed. Null-pointer assignement for coredump \n");
    fflush(stderr);
    /* Should never return, but just in case lib is broken (Android?)
     * make a deliberate null pointer assignment */
    *((int *)NULL) = 1;
#else
    fprintf(stderr, "Exit with failure\n");
    fflush(stderr);
#endif
    /*  Fast-terminate process without signalling and unloading mmap data (i.e.
       shared libraries) */
    _exit(1);
}

static inline void assertfail(char *assertstr,
                              char *filestr, const char *sfun, int line)
{
    notify_failure(_ASSERT, assertstr, sfun, filestr, line);

#ifndef NDEBUG
    /* Generate coredump */
    fprintf(stderr, "Calling abort() for coredump \n");
    fflush(stderr);
    abort();
    fprintf(stderr, "Abort failed. Null-pointer assignment for coredump \n");
    fflush(stderr);
    /* Should never return, but just in case lib is broken (Android?)
     * make a deliberate null pointer assignment */
    *((int *)NULL) = 1;
#else
    fprintf(stderr, "Exit with failure\n");
    fflush(stderr);
#endif
}

/***************************************************************************/
#ifndef assure_h
#define assure_h
/***************************************************************************/

#define _STR(x) #x
#define STR(x) _STR(x)

#if __STDC_VERSION__ < 199901L
# if (__GNUC__ >= 2) && !defined(__func__)
#  define __func__ __FUNCTION__
# endif
#endif

#if defined(__PRETTY_FUNCTION__)
#define _FNC __PRETTY_FUNCTION__
#elif defined (__FUNCTION__)
#define _FNC __FUNCTION__
#elif defined (__func__)
#define _FNC __func__
#elif defined (_func_)
#define _FNC _func_
#endif

#ifdef _FNC
#define FNC _FNC
#else
#define FNC NULL
#endif
/***************************************************************************
 * ASSURE* / EXCEPTION* macros
 ***************************************************************************
 *
 * ASSURE     - Use as assert with the difference that it ignores NDEBUG.
 *              I.e. code inside '(' ')' will never be optimized away AND
 *              process will always fail on failure.
 * ASSURE_E   - Same as above, but has a dedicated exit ability instead of
 *              aborting (second argument). This can be call to error/exception
 *              handling function or semantics like "goto" and "return"
 * ASSERT     - Our version of the assert macro which considers NDEBUG, but if
 *              set it does NOT remove the expanded code which is the only
 *              conceptual difference. I.e. it will NOT terminate process on
 *              failure when NDEBUG is set (as assert does), but it WILL
 *              execute the code (and it will print-out the failure).
 *
 ***************************************************************************/
#include <string.h>

#define FLE strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__

#ifndef __GNUC__
# define ASSURE(p) ((p) ? (void)0 : (void) notify_andfail( \
    _ASSURE, #p, FNC, FLE, __LINE__ ) )
# define ASSURE_E(p,e) if (!(p)) {(void) notify_failure( \
   _ASSURE, #p, FNC, FLE, __LINE__ ); e;}
#else
# define ASSURE(p) ((p) ? (void)0 : (void) notify_andfail( \
    _ASSURE, #p, __FUNCTION__, FLE, __LINE__ ) )
# define ASSURE_E(p,e) if (!(p)) {(void) notify_failure( \
    _ASSURE, #p, __FUNCTION__, FLE, __LINE__ ); e;}
#endif

#define ASSERT assert_ext

/***************************************************************************
 * assrt_* macros (old)
 ***************************************************************************
 *
 * assert_np  - This macro does exactly and what assert does (is assert).
 *              It differs from assert in that if NDEBUG is defined it will NOT
 *              remove arg. I.e. it is a safe macro to use instead of assert as
 *              replacement.
 *              This is a good macro checking return values until design
 *              rigid enough when that isn't needed anymore, when handling
 *              any failure is ignored (by setting NDEBUG).
 * assert_ext - Same as assert_np, but handles NDEBUG differently. It's equal
 *              in the aspect that it will always exit(1) on failure. The
 *              difference is in the print-out format: while assert_np uses
 *              assert, which may look differently on different targets whilst
 *              assert_ext uses a custom macro which behaves the same
 *              regardless of target and NDEBUG setting uses LOGx macros.
 * assert_ret - Returns from function with the result. Assumes the function
 *              returns error-codes. Think twice before using this.
 *
 ***************************************************************************
   Use of the assert_* macros directly is discouraged as they may be
   discontinued/changed.  Use the for new ASSURE* / ASSERT* macros above
   instead.
 ***************************************************************************/

#include <stdio.h>

#ifndef NDEBUG
#include <assert.h>
#  define assert_np assert
#else
#  define assert_np(p) (p)
#endif

#define assert_ext(p) ((p) ? (void)0 : (void) assertfail( \
        #p, FLE , FNC, __LINE__ ) )

/* Lazy error-handling. Careful using this. Assumes function invoked from
accepts returning with code, and that the code means error */
#  define assert_ret(p) (                                  \
    {                                                      \
        int rc = (p);                                      \
                                                           \
        rc ? (void)0 : (void) notify_failure(              \
            #p, __FUNCTION__, FLE, __LINE__ );             \
        return rc;                                         \
    }                                                      \
)

#endif                          /* assure_h */
