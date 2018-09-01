/***************************************************************************
 *   Copyright (C) 2015 by Michael Ambrus                                  *
 *   ambrmi09@gmail.com                                                    *
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
#if !defined(WIN32) && !defined(__CYGWIN__) && defined(LIBLOG_ENABLE_SYSLOG)
#include <syslog.h>
#endif
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <sys/types.h>
#include <time.h>
#include <sys/time.h>
#include <unistd.h>
#include <liblog/log.h>
#include "config.h"
#include "local.h"

#if defined(WIN32) || defined(__CYGWIN__)
#define SYSTEM_SYSLOG_INABLE
#endif

#define ENV_LOG_LEVEL           "LOG_LEVEL"
#define LOG_LINE_MAX_CHARS      2048

/* Global variables that affect assure */
log_level log_ASSURE = LOG_LEVEL_WARNING;
log_level log_ASSERT = LOG_LEVEL_ERROR;

/* Local variables */
static char proc_name[NAME_MAX] = LOG_AS_PROCESS_NAME_DFLT;

#ifdef LIBLOG_ENABLE_SYSLOG
static int syslog_open = 0;
#endif

/*****************************************************************************
  log_level binds when global variable initialization is run in .start, i.e.
  _before_ CTOR which is initialized in initfini.c
 *****************************************************************************/
log_level log_filter_level = DEF_LOG_LEVEL;

int log_getenv_loglevel(void)
{
    int log_level = DEF_LOG_LEVEL;

    if ((getenv(ENV_LOG_LEVEL) != NULL)
        && (strlen(getenv(ENV_LOG_LEVEL)) > 0)) {

        int valid_value;

        log_level = str2loglevel(getenv(ENV_LOG_LEVEL), &valid_value);
        if (!valid_value) {
            log_level = DEF_LOG_LEVEL;
        }
    }
    return log_level;
}

log_level str2loglevel(const char *str, int *ok)
{
    log_level level = LOG_LEVEL_ERROR;
    int valid = 1;

    if ((strnlen(str, NAME_MAX) == 1)
        && (((str[0] - '0') <= LOG_LEVEL_CRITICAL)
            && ((str[0] - '0') >= LOG_LEVEL_VERBOSE))) {

        level = atoi(str);
        goto done;
    }

    if (!strcasecmp(str, "critical")) {
        level = LOG_LEVEL_CRITICAL;
    } else if (!strcasecmp(str, "error")) {
        level = LOG_LEVEL_ERROR;
    } else if (!strcasecmp(str, "warning")) {
        level = LOG_LEVEL_WARNING;
    } else if (!strcasecmp(str, "info")) {
        level = LOG_LEVEL_INFO;
    } else if (!strcasecmp(str, "debug")) {
        level = LOG_LEVEL_DEBUG;
    } else if (!strcasecmp(str, "verbose")) {
        level = LOG_LEVEL_VERBOSE;
    } else {
        valid = 0;
    }

done:
    *ok = valid;
    return level;
}

void log_set_process_name(const char *name)
{
    char *sptr;

    sptr = strrchr(name, '/');
    if (sptr)
        strncpy(proc_name, sptr + 1, NAME_MAX);
    else
        strncpy(proc_name, name, NAME_MAX);

#ifdef LIBLOG_ENABLE_SYSLOG
    log_syslog_config(ENABLE_SYSLOG_STDERR);
#endif
}

void log_write(log_level level, const char *format, ...)
{
    /* Only process this message if its level exceeds the current threshold */
    if (level >= log_filter_level) {
        va_list args;

        /* Write the log message */
        va_start(args, format);
#if defined(SYSTEM_SYSLOG_INABLE) || ! defined(LIBLOG_ENABLE_SYSLOG)
        {
            char tstr[LOG_LINE_MAX_CHARS];
            char buffer[LOG_LINE_MAX_CHARS];
            struct tm tm;
            struct timeval now;

            gettimeofday(&now, NULL);

            strftime(tstr, LOG_LINE_MAX_CHARS, "%y%m%d-%H%M%S",
                     localtime_r(&(now.tv_sec), &tm));
            snprintf(buffer, LOG_LINE_MAX_CHARS, "%s.%06lu %s[%d]: %s", tstr,
                     now.tv_usec, proc_name, getpid(), format);

            vfprintf(stderr, buffer, args);
        }
#else
        {
            int syslog_level;

            switch (level) {
                case LOG_LEVEL_VERBOSE:
                case LOG_LEVEL_DEBUG:
                    syslog_level = LOG_DEBUG;
                    break;

                case LOG_LEVEL_INFO:
                    syslog_level = LOG_INFO;
                    break;

                case LOG_LEVEL_WARNING:
                    syslog_level = LOG_WARNING;
                    break;

                case LOG_LEVEL_ERROR:
                    syslog_level = LOG_ERR;
                    break;

                case LOG_LEVEL_CRITICAL:
                    syslog_level = LOG_CRIT;
                    break;

                default:
                    /* Shouldn't be used, so just route it to a low level */
                    syslog_level = LOG_DEBUG;
                    break;
            }

            vsyslog(syslog_level | LOG_USER, format, args);
        }
#endif
        va_end(args);
    }
}

void log_set_verbosity(log_level level)
{
    log_filter_level = level;
}

#ifdef LIBLOG_ENABLE_SYSLOG
void log_syslog_config(int incl_stderr)
{
    if (syslog_open) {
        closelog();
        syslog_open = 0;
    }

    if (incl_stderr) {

        openlog(proc_name,
                LOG_CONS | LOG_NDELAY | LOG_NOWAIT | LOG_PERROR | LOG_PID,
                LOG_USER);
    } else {
        openlog(proc_name,
                LOG_CONS | LOG_NDELAY | LOG_NOWAIT | LOG_PID, LOG_USER);
    }
    syslog_open = 1;
}
#endif

/* Test what log-level is effectuated and where it ends up */
void log_test()
{
    LOGV("liblog-test [VERBOSE] %d\n", LOG_LEVEL_VERBOSE);
    LOGD("liblog-test [DEBUG] %d\n", LOG_LEVEL_DEBUG);
    LOGI("liblog-test [INFO] %d\n", LOG_LEVEL_INFO);
    LOGW("liblog-test [WARNING] %d\n", LOG_LEVEL_WARNING);
    LOGE("liblog-test [ERROR] %d\n", LOG_LEVEL_ERROR);
}
