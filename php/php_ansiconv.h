/*
  +----------------------------------------------------------------------+
  | PHP Version 5                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2004 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.0 of the PHP license,       |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_0.txt.                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author:                                                              |
  +----------------------------------------------------------------------+
*/

/* $Id: header,v 1.15 2004/01/08 16:46:52 sniper Exp $ */

#ifndef PHP_ANSICONV_H
#define PHP_ANSICONV_H

#include <ansiconv/ansiconv.h>

extern zend_module_entry ansiconv_module_entry;
#define phpext_ansiconv_ptr &ansiconv_module_entry

#ifdef PHP_WIN32
#define PHP_ANSICONV_API __declspec(dllexport)
#else
#define PHP_ANSICONV_API
#endif

#ifdef ZTS
#include "TSRM.h"
#endif

PHP_MINIT_FUNCTION(ansiconv);
PHP_MSHUTDOWN_FUNCTION(ansiconv);
PHP_RINIT_FUNCTION(ansiconv);
PHP_RSHUTDOWN_FUNCTION(ansiconv);
PHP_MINFO_FUNCTION(ansiconv);

PHP_FUNCTION(ansiconv_convert);

#ifdef ZTS
#define ANSICONV_G(v) TSRMG(ansiconv_globals_id, zend_ansiconv_globals *, v)
#else
#define ANSICONV_G(v) (ansiconv_globals.v)
#endif

#endif	/* PHP_ANSICONV_H */


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
