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
#ifndef log_h
#define log_h

/**
 * Severity levels for logging functions
 */
typedef enum {
    LOG_LEVEL_VERBOSE,          /* Verbose level logging */
    LOG_LEVEL_DEBUG,            /* Debug level logging */
    LOG_LEVEL_INFO,             /* Information level logging */
    LOG_LEVEL_WARNING,          /* Warning level logging */
    LOG_LEVEL_ERROR,            /* Error level logging */
    LOG_LEVEL_CRITICAL,         /* Fatal error level logging */
    LOG_LEVEL_SILENT            /* No output */
} log_level;

void log_write(log_level level, const char *format, ...);
void log_set_verbosity(log_level level);
log_level str2loglevel(const char *str, int *ok);
int log_getenv_loglevel(void);
void log_test(void);
void log_syslog_config(int incl_stderr);
void log_set_process_name(const char *name);

/* Bladerf compatible macros (too much to type though) */
#define log_verbose(...)	log_write(LOG_LEVEL_VERBOSE, "[V] "__VA_ARGS__)
#define log_debug(...)		log_write(LOG_LEVEL_DEBUG, "[D] "__VA_ARGS__)
#define log_info(...)		log_write(LOG_LEVEL_INFO, "[I] "__VA_ARGS__)
#define log_warning(...)	log_write(LOG_LEVEL_WARNING, "[W] "__VA_ARGS__)
#define log_error(...)		log_write(LOG_LEVEL_ERROR, "[E] "__VA_ARGS__)
#define log_critical(...)	log_write(LOG_LEVEL_CRITICAL, "[C] "__VA_ARGS__)

/* Android translation of the macros above (smoother)*/
#define LOGV log_verbose
#define LOGD log_debug
#define LOGI log_info
#define LOGW log_warning
#define LOGE log_error

#endif                          //log_h
