# SPDX-License-Identifier: GPL-3.0
# Copyright (C) 2025 Vgwws
#
# This file is licensed under the GPL-3.0 License. See LICENSE for details.

lua_libdir = /
lua_lib_LTLIBRARIES = libluacrunum.la

libluacrunum_la_SOURCES = crunum.c matrix.c vector.c
libluacrunum_la_CPPFLAGS = -I$(top_srcdir)/include $(LUA_CFLAGS)
libluacrunum_la_CFLAGS = @CFLAGS@ $(LUA_CFLAGS)
libluacrunum_la_LDFLAGS = $(LUA_LIBS)
libluacrunum_la_LIBADD = $(top_srcdir)/src/core/libcrunum.la

install:
	@printf "copy/move/symlink the library instead manually,\n"
	@printf "and rename it into crunum.so\n"

uninstall:
	@printf "remove the library manually instead\n"
