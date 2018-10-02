/***************************************************************************
 *   Copyright (C) 2018 by Michael Ambrus                                  *
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
/*
 * Definitions that should not be in public / installed header
 */

#ifndef log_local_h
#define log_local_h

#include <liblog/log.h>
#include "config.h"

#ifndef DEF_LOG_LEVEL
#  ifdef LIBLOG_ENABLE_SYSLOG
#    define DEF_LOG_LEVEL LOG_LEVEL_INFO
#  else
#    define DEF_LOG_LEVEL LOG_LEVEL_WARNING
#  endif                        // LIBLOG_ENABLE_SYSLOG
#endif                          // LIBLOG_DEFAULT_LOG_LEVEL

#endif                          // log_local_h
