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
#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <string.h>
#include <syslog.h>
#include <liblog/log.h>
#include "config.h"
#define __init __attribute__((constructor))
#define __fini __attribute__((destructor))

#ifndef PROJ_NAME
#define PROJ_NAME LIBLOG_PROJ_NAME
#endif

#ifndef VERSION
#define VERSION LIBLOG_VERSION
#endif

/* Module initializers/deinitializers. When used as library (who don't have
 * a natural entry/exit function) these are used to initialize
 * deinitialize. Use to set predefined/default states and cleanup.
 *
 * This will work with shared libraries as well as with static as they get
 * invoked by RTL load/unload, with or without C++ code (i.e. functions will
 * play nice with C++ normal ctors/dtors).
 *
 * Keep log in to at least once per new build-/run-environment assert that
 * the mechanism works.
 */
extern log_level log_filter_level;

void __init __liblog_init(void)
{
    int log_level;

    log_filter_level = DEF_LOG_LEVEL;   /* Global variable initializer, if missed */
    log_level = log_getenv_loglevel();

    /* Open syslog, include stderr in output */
    log_syslog_config(1);

    log_set_verbosity(log_level);
    log_debug("%s %s: initializing\n", PROJ_NAME, VERSION);
}

void __fini __liblog_fini(void)
{
    int log_level = log_getenv_loglevel();

    log_set_verbosity(log_level);
    log_debug("% %s: deinitializing\n", PROJ_NAME, VERSION);
    fflush(NULL);
    closelog();
}
