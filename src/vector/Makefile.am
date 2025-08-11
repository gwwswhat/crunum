# SPDX-License-Identifier: GPL-3.0
# Copyright (C) 2025 Vgwws
#
# This file is licensed under the GPL-3.0 License. See LICENSE for details.

vector_libdir = @libdir@/crunum
vector_lib_LTLIBRARIES = libvector.la

libvector_la_SOURCES = vector_core.c vector_math.c
libvector_la_CPPFLAGS = -I$(top_srcdir)/include
libvector_la_CFLAGS = @CFLAGS@
libvector_la_LDFLAGS = -version-info 0:0:0 -no-undefined
