cmake_minimum_required (VERSION 2.6)

project (liblog C)

set (LIBLOG_VERSION_MAJOR 0)
set (LIBLOG_VERSION_MINOR 3)
set (LIBLOG_VERSION_PATCH 1)

include_directories("${PROJECT_BINARY_DIR}")
include_directories("${PROJECT_SOURCE_DIR}")
include_directories("${PROJECT_BINARY_DIR}/include")
include_directories("${PROJECT_SOURCE_DIR}/include")

set(LOGGING_SOURCE
	log.c
	error.c
	initfini.c
)

# Prevent initfini functions from being opt out
set(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} -Wl,--undefined=__liblog_init")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,--undefined=__liblog_init")

option (ENABLE_INITFINI_SHOWEXEC "Show init/fini execution" OFF)
option (LIBLOG_ENABLE_SYSLOG "Log to syslog" OFF)
option (LIBLOG_ASSURE_LOGGED "Error functions uses liblog for pritouts" ON)
option (LIBLOG_ASSURE_INCL_FILEINFO "File-name/line-number appended" OFF)

set(LIBLOG_DEFAULT_LOG_LEVEL
	${LIBLOG_DEFAULT_LOG_LEVEL}
	CACHE STRING
	"Set explicit default log-level: [0-6] | [VERBOSE|DEBUG|INFO|WARNING|ERROR|CRITICAL|SILENT]")

if(LIBLOG_DEFAULT_LOG_LEVEL STREQUAL "")
	set (CMAKE_DEF_LIBLOG_DEFAULT_LOG_LEVEL OFF)
else()
	set (CMAKE_DEF_LIBLOG_DEFAULT_LOG_LEVEL ON)

	string(TOUPPER ${LIBLOG_DEFAULT_LOG_LEVEL} LIBLOG_DEFAULT_LOG_LEVEL)

	if (NOT LIBLOG_DEFAULT_LOG_LEVEL MATCHES "^[0-9]+$")
		string(CONCAT
			LIBLOG_DEFAULT_LOG_LEVEL
			"LOG_LEVEL_" ${LIBLOG_DEFAULT_LOG_LEVEL})
	endif ()
endif()


if (LIBLOG_ENABLE_SYSLOG)
	option(ENABLE_SYSLOG_STDERR
		"Send logs also to stderr" ON)

endif (LIBLOG_ENABLE_SYSLOG)

configure_file (
	"${PROJECT_SOURCE_DIR}/config.h.in"
	"${PROJECT_BINARY_DIR}/config.h"
)

add_library(log        SHARED ${LOGGING_SOURCE})
add_library(log_static STATIC ${LOGGING_SOURCE})

set_target_properties(
  log PROPERTIES PUBLIC_HEADER
  "include/liblog/log.h;include/liblog/assure.h")

# Installation
# ------------

# Shells (ignore for now)
#install(PROGRAMS ${PROJECT_BINARY_DIR}/blaha.sh DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)

# Targets:
install(TARGETS log log_static
        RUNTIME       DESTINATION ${CMAKE_INSTALL_PREFIX}/bin
        LIBRARY       DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
        ARCHIVE       DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
        PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_PREFIX}/include/${PROJECT_NAME})
