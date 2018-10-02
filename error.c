/***************************************************************************
 *   Copyright (C) 2015, 2018 by Michael Ambrus                            *
 *   michael@helsinova.se                                                  *
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
 ***************************************************************************/
#include <strings.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <sys/types.h>
#include <time.h>
#include <sys/time.h>
#include <unistd.h>
#include <liblog/assure.h>
#include "config.h"
#include "local.h"
#if !defined(WIN32) && !defined(__CYGWIN__) && defined(LIBLOG_ENABLE_SYSLOG)
#include <syslog.h>
#endif

#if defined(WIN32) || defined(__CYGWIN__)
#define SYSTEM_SYSLOG_INABLE
#endif

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

void liblog_print_failure(assrt_t etype, char *sassure,
                           const char *sfun, char *sfile, int iline)
{
#ifdef LIBLOG_ASSURE_LOGGED
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

#  ifdef LIBLOG_ASSURE_INCL_FILEINFO
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
