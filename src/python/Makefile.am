# SPDX-License-Identifier: GPL-3.0
# Copyright (C) 2025 Vgwws
#
# This file is licensed under the GPL-3.0 License. See LICENSE for details.

python_pkgdir = @PYTHON_SITE_PACKAGES@
python_pkg_LTLIBRARIES = libcrunum.la

libcrunum_la_SOURCES = crunum.c matrix.c vector.c
libcrunum_la_CPPFLAGS = -I$(top_srcdir)/include
libcrunum_la_CFLAGS = @CFLAGS@ $(PYTHON_CFLAGS)
libcrunum_la_LDFLAGS = -module -avoid-version $(PYTHON_LIBS)
libcrunum_la_LIBADD = $(top_srcdir)/src/matrix/libmatrix.la \
											$(top_srcdir)/src/vector/libvector.la

install-exec-local:
	ln -sf $(DESTDIR)$(python_pkgdir)/libcrunum.so $(DESTDIR)$(python_pkgdir)/crunum.so

uninstall-local:
	rm -f $(DESTDIR)$(python_pkgdir)/crunum.so
