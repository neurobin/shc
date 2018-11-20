AC_INIT([shc], [4.0.1], [http://github.com/neurobin/shc/issues])
AC_CONFIG_AUX_DIR(config)
#prefix="/usr"
AC_CONFIG_SRCDIR([src/shc.c])


AM_INIT_AUTOMAKE

# Checks for programs.
AC_PROG_CC
AC_PROG_INSTALL

# Checks for libraries.

# Checks for header files.
AC_CHECK_HEADERS([fcntl.h stdlib.h string.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_CHECK_MEMBERS([struct stat.st_rdev])

# Checks for library functions.
AC_FUNC_MALLOC
AC_FUNC_MKTIME
AC_FUNC_REALLOC
AC_CHECK_FUNCS([memset putenv strchr strdup strrchr])


AC_OUTPUT(Makefile src/Makefile)
