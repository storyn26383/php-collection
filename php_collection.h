#ifndef _PHP_COLLECTION_H
    #define _PHP_COLLECTION_H

    #ifdef HAVE_CONFIG_H
        #include "config.h"
    #endif

    #ifdef ZTS
        #include <TSRM.h>
    #endif

    #include <php.h>

    extern zend_module_entry collection_module_entry;

    PHP_MINIT_FUNCTION(collection);
    PHP_MSHUTDOWN_FUNCTION(collection);
    PHP_MINFO_FUNCTION(collection);

    PHP_FUNCTION(collect);
    PHP_METHOD(Collection, __construct);
#endif
