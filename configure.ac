AC_COPYRIGHT([
							SPDX-License-Identifier: GPL-3.0
							Copyright (C) 2025 Vgwws
							This file is licensed under the GPL-3.0 License. See LICENSE for details.
							])
AC_INIT([crunum], [0.0.0], [gwwswhat@gmail.com])
AC_SUBST([VERSION], [0.0.0])
AM_INIT_AUTOMAKE([-Wall subdir-objects])
AM_PROG_AR
LT_INIT
AC_CONFIG_SRCDIR([src/core/matrix_core.c])
AC_CONFIG_MACRO_DIRS([m4])
AC_CONFIG_HEADERS([include/config.h])

AC_ARG_WITH([target],
	[AS_HELP_STRING([--with-target=language], [Specify target language])],
	[target_lang="$withval"],
	[target_lang=""])

if test -z "$target_lang"; then
	AC_MSG_ERROR([Must specify --with-target=<language>])
fi

AM_CONDITIONAL([TARGET_PYTHON], [test "x$target_lang" = "xpython"])
AM_CONDITIONAL([TARGET_LUA], [test "x$target_lang" = "xlua"])
AM_CONDITIONAL([TARGET_C], [test "x$target_lang" = "xc"])

AC_CANONICAL_HOST
AC_PROG_CC

AX_CHECK_COMPILE_FLAG([-Wall],
	[CFLAGS="$CFLAGS -Wall"],
	[])
AX_CHECK_COMPILE_FLAG([-Wextra],
	[CFLAGS="$CFLAGS -Wextra"],
	[])
AX_CHECK_COMPILE_FLAG([-Werror],
	[CFLAGS="$CFLAGS -Werror"],
	[])
AX_CHECK_COMPILE_FLAG([-std=gnu99],
	[CFLAGS="$CFLAGS -std=gnu99"],
	[AC_MSG_ERROR([No -std=gnu99 support])])

AC_ARG_ENABLE([debug],
	[AS_HELP_STRING([--enable-debug], [Enable debug build flags])],
	[enable_debug=yes],
	[enable_debuf=no])

if test "x$enable_debug" = "xyes"; then
	AX_CHECK_COMPILE_FLAG([-g],
		[CFLAGS="$CFLAGS -g"],
		[])
	AX_CHECK_COMPILE_FLAG([-O0],
		[CFLAGS="$CFLAGS -O0"],
		[])
	AC_DEFINE([DEBUG], [], [Define debug mode])
else
	AX_CHECK_COMPILE_FLAG([-O2],
		[CFLAGS="$CFLAGS -O2"],
		[])
fi

case "$host_cpu" in
	arm*)
		AX_CHECK_COMPILE_FLAG([-mfpu=neon],
			[
			 CFLAGS="$CFLAGS -mfpu=neon"
			 AC_DEFINE([HAVE_NEON], [1], [Define if CPU have NEON support])
			 AC_DEFINE([HAVE_SIMD], [1], [Define if CPU have SIMD support])
			 AX_FUNC_POSIX_MEMALIGN
			],
			[])
		;;
	aarch64)
		AC_DEFINE([HAVE_NEON], [1], [Define if CPU have NEON support])
		AC_DEFINE([HAVE_SIMD], [1], [Define if CPU have SIMD support])
		AX_FUNC_POSIX_MEMALIGN
		;;
	*)
		;;
esac

AC_SUBST([CFLAGS])

AC_PROG_INSTALL
AC_PROG_LN_S

AC_C_INLINE

AC_FUNC_MALLOC
AC_FUNC_REALLOC

AC_CHECK_HEADERS([stdlib.h string.h math.h time.h stddef.h], [], [
	AC_MSG_ERROR([Can't find required headers])])
AC_CHECK_FUNCS([srand time rand], [], [
	AC_MSG_ERROR([Can't find required functions])])

PKG_PROG_PKG_CONFIG

m4_define([CHECK_LUA_MODULES], [
					 LUA_PKGS="lua5.1 lua-5.1 lua51 lua5.2 lua-5.2 lua52 lua5.3 lua-5.3 lua53 lua5.4 lua-5.4 lua54"
					 for pkg in $LUA_PKGS; do
						 PKG_CHECK_MODULES([LUA], [$pkg], [
							AC_PATH_PROG([LUA], [$pkg lua])
							break
							], true)
					 done
					 if test "x$LUA_LIBS" = "x"; then
						 AC_MSG_ERROR([Could not find any pkg-config for lua])
					 fi
					 LUA_CPATH=`$LUA -e 'print(package.cpath)'`
					 LUA_CPATH=`echo $LUA_CPATH | cut -d';' -f1 | sed 's/?.so//g'`
					 AC_SUBST([LUA_CPATH])
					 ])

AC_MSG_CHECKING([target language])
AC_MSG_RESULT([$target_lang])

case "$target_lang" in
	python)
		PKG_CHECK_MODULES([PYTHON], [python3], [],
			[AC_MSG_ERROR([Can't find pkg-config for python3])])
		AC_PATH_PROG([PYTHON], [python3 python])
		PYTHON_SITE_PACKAGES=`$PYTHON -m site --user-site`
		AC_SUBST([PYTHON_SITE_PACKAGES])
		;;
	lua)
		CHECK_LUA_MODULES
		;;
	c)
		;;
	*)
		AC_MSG_ERROR([Unknown target language])
		;;
esac
AC_SUBST([target_lang])

AC_CONFIG_FILES([Makefile include/Makefile src/Makefile src/core/Makefile src/lua/Makefile src/python/Makefile pkgconfig/Makefile tests/Makefile tests/lua/Makefile tests/python/Makefile pkgconfig/crunum.pc])

AC_OUTPUT
