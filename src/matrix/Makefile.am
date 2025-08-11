# SPDX-License-Identifier: GPL-3.0
# Copyright (C) 2025 Vgwws
#
# This file is licensed under the GPL-3.0 License. See LICENSE for details.

matrix_libdir = @libdir@/crunum
matrix_lib_LTLIBRARIES = libmatrix.la

libmatrix_la_SOURCES = matrix_core.c matrix_math.c
libmatrix_la_CPPFLAGS = -I$(top_srcdir)/include
libmatrix_la_CFLAGS = @CFLAGS@
libmatrix_la_LDFLAGS = -version-info 0:0:0 -no-undefined
