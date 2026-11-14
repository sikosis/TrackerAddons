## Haiku Generic Makefile v2.6 ##

NAME = ImageMounter
TARGET_DIR = .
TYPE = APP
APP_MIME_SIG = application/x-vnd.ImageMounter

SRCS = main.cpp MountPoint.cpp
RDEFS = ImageMounter.rdef
RSRCS =

LIBS = be tracker $(STDCPPLIBS)
LIBPATHS =
SYSTEM_INCLUDE_PATHS =
LOCAL_INCLUDE_PATHS =

OPTIMIZE = FULL
LOCALES =
DEFINES =
WARNINGS = ALL
SYMBOLS =
DEBUGGER =
COMPILER_FLAGS = -std=c++17
LINKER_FLAGS =
APP_VERSION =
DRIVER_PATH =

TRACKER_ADDONS_DIR = \
	$(shell finddir B_USER_NONPACKAGED_ADDONS_DIRECTORY)/Tracker
INSTALL_DIR = $(TRACKER_ADDONS_DIR)

DEVEL_DIRECTORY := \
	$(shell findpaths -r "makefile_engine" B_FIND_PATH_DEVELOP_DIRECTORY)
include $(DEVEL_DIRECTORY)/etc/makefile-engine
