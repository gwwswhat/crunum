AC_COPYRIGHT([
							Copyright (C) 2025 Vgwws
							Licensed under the GPL-3.0 License.
							])
AC_INIT([crunum], [0.0.0], [gwwswhat@gmail.com])
AM_INIT_AUTOMAKE([-Wall subdir-objects])
AM_PROG_AR
LT_INIT
AC_CONFIG_SRCDIR([src/matrix/matrix_core.c])
AC_CONFIG_MACRO_DIRS([m4])
AC_CONFIG_HEADERS([config.h])
AC_ARG_ENABLE([debug],
							[AS_HELP_STRING([--enable-debug], [Enable debug build flags])],
							[enable_debug=$enableval],
						  [enable_debug=no])

if test "x$enable_debug" = "xyes"; then
	CFLAGS="$CFLAGS -Wall -Wextra -Werror -std=gnu99 -fPIC -g -O0 -DDEBUG"
	AC_SUBST([CFLAGS]) 
else
	CFLAGS="$CFLAGS -Wall -Wextra -Werror -std=gnu99 -fPIC -O2"
	AC_SUBST([CFLAGS])
fi

AC_ARG_WITH([target],
						[AS_HELP_STRING([--with-target=language], [Specify target language])],
						[target_lang="$withval"],
						[target_lang=""])

if test -z "$target_lang"; then
	AC_MSG_ERROR([Must specify --with-target=<language>])
fi

AM_CONDITIONAL([TARGET_PYTHON], [test "x$target_lang" = "xpython"])
AM_CONDITIONAL([TARGET_LUA], [test "x$target_lang" = "xlua"])

AC_PROG_CC
AC_PROG_INSTALL

AC_C_INLINE

AC_FUNC_MALLOC
AC_FUNC_REALLOC

AC_CHECK_HEADERS([stdlib.h string.h math.h time.h stddef.h], [], [
									AC_MSG_ERROR([Can't find required headers])
								  ])
AC_CHECK_FUNCS([srand time rand], [], [
								AC_MSG_ERROR([Can't find required functions])
								])

AC_MSG_CHECKING([whether CPU have NEON support])
saved_CFLAGS="$CFLAGS"
CFLAGS="-march=native"
AC_COMPILE_IFELSE(
	[AC_LANG_PROGRAM(
		[[]], [[
		 #if defined(__ARM_NEON) || defined(__ARM_NEON__)
		 int main(void){
			 return 0;
		 }
	   #else
		 #error "NEON not supported"
		 #endif
		 ]])],
	[
	 AC_MSG_RESULT([yes])
	 AC_DEFINE([HAVE_NEON], [1], [Define if CPU have NEON support])
	 simd_supported=yes
	], 
	[
	 AC_MSG_RESULT([no])
	 simd_supported=no
	])
CFLAGS="$saved_CFLAGS"

if test "x$simd_supported" = "xyes"; then
	AX_FUNC_POSIX_MEMALIGN
fi

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
					 LUA_CPATH_FIRST=`echo '$LUA_CPATH' | cut -d';' -f1 | sed 's/?.so//g'`
					 AC_SUBST([LUA_CPATH_FIRST])
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
	*)
	  AC_MSG_ERROR([Unknown target language])
		;;
esac

AC_CONFIG_FILES([Makefile 
								 src/Makefile 
								 src/matrix/Makefile 
								 src/vector/Makefile 
								 src/"$target_lang"/Makefile])

AC_OUTPUT
