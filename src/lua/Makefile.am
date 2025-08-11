lua_pkgdir = @LUA_CPATH_FIRST@
lua_pkg_LTLIBRARIES = libcrunum.la

libcrunum_la_SOURCES = crunum.c matrix.c vector.c
libcrunum_la_CPPFLAGS = -I$(top_srcdir)/include $(LUA_CFLAGS)
libcrunum_la_CFLAGS = @CFLAGS@ $(LUA_CFLAGS)
libcrunum_la_LDFLAGS = $(LUA_LIBS)
libcrunum_la_LIBADD = $(top_srcdir)/src/matrix/libmatrix.la \
											$(top_srcdir)/src/vector/libvector.la

install-exec-local:
	ln -sf $(DESTDIR)$(lua_pkgdir)/libcrunum.so $(DESTDIR)$(lua_pkgdir)/crunum.so

uninstall-local:
	rm -f $(DESTDIR)$(lua_pkgdir)/crunum.so
