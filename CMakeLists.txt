cmake_minimum_required (VERSION 2.6)

project (liblog C)

set (LIBLOG_VERSION_MAJOR 0)
set (LIBLOG_VERSION_MINOR 3)
set (LIBLOG_VERSION_PATCH 0)

include_directories("${PROJECT_BINARY_DIR}")
include_directories("${PROJECT_SOURCE_DIR}")
include_directories("${PROJECT_BINARY_DIR}/include")
include_directories("${PROJECT_SOURCE_DIR}/include")

set(LOGGING_SOURCE
	log.c
	initfini.c
)

# Prevent initfini functions from being opt out
set(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} -Wl,--undefined=__liblog_init")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,--undefined=__liblog_init")

option (LIBLOG_ENABLE_SYSLOG "Log to syslog" OFF)

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
