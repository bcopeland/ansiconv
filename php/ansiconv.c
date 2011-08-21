/*
 * Ansiconv php module 
 * (C) 2006 Bob Copeland  <me@bobcopeland.com> 
 * GPL v2
 */

/* $Id: header,v 1.15 2004/01/08 16:46:52 sniper Exp $ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "php_ansiconv.h"

/* If you declare any globals in php_ansiconv.h uncomment this:
ZEND_DECLARE_MODULE_GLOBALS(ansiconv)
*/

/* True global resources - no need for thread safety here */
static int le_ansiconv;

/* {{{ ansiconv_functions[]
 *
 * Every user visible function must have an entry in ansiconv_functions[].
 */
function_entry ansiconv_functions[] = {
	PHP_FE(ansiconv_convert,	NULL)
	{NULL, NULL, NULL}	/* Must be the last line in ansiconv_functions[] */
};
/* }}} */

/* {{{ ansiconv_module_entry
 */
zend_module_entry ansiconv_module_entry = {
#if ZEND_MODULE_API_NO >= 20010901
	STANDARD_MODULE_HEADER,
#endif
	"ansiconv",
	ansiconv_functions,
	PHP_MINIT(ansiconv),
	PHP_MSHUTDOWN(ansiconv),
	NULL,
	NULL,
	PHP_MINFO(ansiconv),
#if ZEND_MODULE_API_NO >= 20010901
	"0.1", /* Replace with version number for your extension */
#endif
	STANDARD_MODULE_PROPERTIES
};
/* }}} */

#ifdef COMPILE_DL_ANSICONV
ZEND_GET_MODULE(ansiconv)
#endif

/* {{{ PHP_INI
 */
/* Remove comments and fill if you need to have entries in php.ini
PHP_INI_BEGIN()
    STD_PHP_INI_ENTRY("ansiconv.global_value",      "42", PHP_INI_ALL, OnUpdateLong, global_value, zend_ansiconv_globals, ansiconv_globals)
    STD_PHP_INI_ENTRY("ansiconv.global_string", "foobar", PHP_INI_ALL, OnUpdateString, global_string, zend_ansiconv_globals, ansiconv_globals)
PHP_INI_END()
*/
/* }}} */

/* {{{ PHP_MINIT_FUNCTION
 */
PHP_MINIT_FUNCTION(ansiconv)
{
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION
 */
PHP_MSHUTDOWN_FUNCTION(ansiconv)
{
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION(ansiconv)
{
	php_info_print_table_start();
	php_info_print_table_header(2, "ansiconv support", "enabled");
	php_info_print_table_end();
}
/* }}} */

/* {{{ proto int ansiconv_convert(resource in, int in_format, 
  		int out_format, [bool thumbnail); */
PHP_FUNCTION(ansiconv_convert)
{
	zval *arg1;
	php_stream *stream;
	long input_format;
	long output_format;
	zend_bool thumbnail = 0;
	FILE *fp;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rll|b", 
			&arg1, &input_format, &output_format, &thumbnail) == FAILURE) {
		RETURN_FALSE;
	}

	php_stream_from_zval_no_verify(stream, &arg1);
	if (stream == NULL) {
		RETURN_FALSE;
	}

	if (php_stream_cast(stream, PHP_STREAM_AS_STDIO | PHP_STREAM_CAST_TRY_HARD,
				(void **) &fp, REPORT_ERRORS) == FAILURE) {
		RETURN_FALSE;
	}
	ansiconv_convert(fp, (int) input_format, (int) output_format, 
			(int) thumbnail);
	RETURN_TRUE;
}
/* }}} */


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
