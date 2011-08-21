dnl $Id$
dnl config.m4 for extension ansiconv

dnl If your extension references something external, use with:

PHP_ARG_WITH(ansiconv, for ansiconv support,
[  --with-ansiconv             Include ansiconv support])

dnl Otherwise use enable:

if test "$PHP_ANSICONV" != "no"; then
  dnl # --with-ansiconv -> check with-path
  SEARCH_PATH="/usr/local /usr"     
  SEARCH_FOR="/include/ansiconv/ansiconv.h" 
  if test -r $PHP_ANSICONV/$SEARCH_FOR; then # path given as parameter
    ANSICONV_DIR=$PHP_ANSICONV
  else # search default path list
    AC_MSG_CHECKING([for ansiconv files in default path])
    for i in $SEARCH_PATH ; do
      if test -r $i/$SEARCH_FOR; then
        ANSICONV_DIR=$i
        AC_MSG_RESULT(found in $i)
      fi
    done
  fi

  if test -z "$ANSICONV_DIR"; then
    AC_MSG_RESULT([not found])
    AC_MSG_ERROR([Please reinstall the ansiconv distribution])
  fi

  dnl # --with-ansiconv -> add include path
  PHP_ADD_INCLUDE($ANSICONV_DIR/include)

  dnl # --with-ansiconv -> check for lib and symbol presence
  LIBNAME=ansiconv
  LIBSYMBOL=ansiconv_convert

  PHP_CHECK_LIBRARY($LIBNAME,$LIBSYMBOL,
  [
    PHP_ADD_LIBRARY_WITH_PATH($LIBNAME, $ANSICONV_DIR/lib, ANSICONV_SHARED_LIBADD)
    AC_DEFINE(HAVE_ANSICONVLIB,1,[ ])
  ],[
    AC_MSG_ERROR([wrong ansiconv lib version or lib not found])
  ],[
    -L$ANSICONV_DIR/lib -lm -ldl
  ])
  
  PHP_SUBST(ANSICONV_SHARED_LIBADD)

  PHP_NEW_EXTENSION(ansiconv, ansiconv.c, $ext_shared)
fi
