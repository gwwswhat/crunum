# SPDX-License-Identifier: GPL-3.0
# Copyright (C) 2025 Vgwws
#
# This file is licensed under the GPL-3.0 License. See LICENSE for details.

python_libdir = /
python_lib_LTLIBRARIES = libpycrunum.la

libpycrunum_la_SOURCES = crunum.c matrix.c vector.c
libpycrunum_la_CPPFLAGS = -I$(top_srcdir)/include
libpycrunum_la_CFLAGS = @CFLAGS@ $(PYTHON_CFLAGS)
libpycrunum_la_LDFLAGS = -module -avoid-version $(PYTHON_LIBS)
libpycrunum_la_LIBADD = $(top_srcdir)/src/core/libcrunum.la

install:
	@printf "copy/move/symlink the library instead manually,\n"
	@printf "and rename it into crunum.so\n"

uninstall:
	@printf "remove the library manually instead\n"
