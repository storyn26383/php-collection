#include "php_collection.h"

#if COMPILE_DL_COLLECTION
    ZEND_GET_MODULE(collection)
#endif

ZEND_BEGIN_ARG_INFO_EX(arginfo_construct, 0, 0, 0)
    ZEND_ARG_INFO(0, items)
ZEND_END_ARG_INFO()

const zend_function_entry collection_functions[] = {
    /* PHP_FE(collect, NULL) */
    PHP_FE_END
};

zend_module_entry collection_module_entry = {
    STANDARD_MODULE_HEADER,
    "Collection",                       // your extension name
    collection_functions,               // where you define your functions
    PHP_MINIT(collection),              // for module initialization
    PHP_MSHUTDOWN(collection),          // for module shutdown process
    NULL, // PHP_RINIT(collection)      // for request initialization
    NULL, // PHP_RSHUTDOWN(collection)  // for reqeust shutdown process
    PHP_MINFO(collection),              // for providing module information
    "0.1.0",                            // your extension version
    STANDARD_MODULE_PROPERTIES
};

zend_class_entry *collection_class_entry;

PHP_MINIT_FUNCTION(collection) {
    zend_class_entry ce;
    zend_function_entry methods[] = {
        PHP_ME(Collection, __construct, arginfo_construct, ZEND_ACC_PUBLIC|ZEND_ACC_CTOR)
        PHP_ME(Collection, sum, NULL, ZEND_ACC_PUBLIC)
        PHP_ME(Collection, avg, NULL, ZEND_ACC_PUBLIC)
        PHP_ME(Collection, count, NULL, ZEND_ACC_PUBLIC)
        PHP_FE_END
    };

    INIT_CLASS_ENTRY(ce, "Collection", methods);

    collection_class_entry = zend_register_internal_class(&ce);

    zval *items;

    zend_declare_property(collection_class_entry, "items", sizeof("items") - 1, items, ZEND_ACC_PROTECTED);

    return SUCCESS;
}

PHP_MSHUTDOWN_FUNCTION(collection) {
    return SUCCESS;
}

PHP_MINFO_FUNCTION(collection) {
    //
}

#define METHOD(name) PHP_METHOD(Collection, name)

#define GET_PROP(value, key) do { \
    zval *rv; \
    value = zend_read_property(collection_class_entry, getThis(), key, sizeof(key) - 1, 1, rv); \
} while (0)

zval php_collection_sum(zval *array) {
    zval *entry, entry_n, result;

    ZVAL_DOUBLE(&result, 0);

    ZEND_HASH_FOREACH_VAL(Z_ARRVAL_P(array), entry) {
        ZVAL_COPY(&entry_n, entry);
        convert_scalar_to_number(&entry_n);
        fast_add_function(&result, &result, &entry_n);
    } ZEND_HASH_FOREACH_END();

    return result;
}

zend_long php_collection_count(zval *array) {
    return zend_array_count(Z_ARRVAL_P(array));
}

// Your functions here...
/* PHP_FUNCTION(collect) { */
/*     RETURN_STRING("Hello Sasaya"); */
/* } */

METHOD(__construct) {
    zval *items;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "|a", &items) == FAILURE) {
        RETURN_NULL();
    }

    if (Z_TYPE_P(items) != IS_ARRAY) {
        ZVAL_NEW_ARR(items);
    }

    zend_update_property(collection_class_entry, getThis(), "items", sizeof("items") - 1, items);
}

METHOD(sum) {
    zval *items;

    if (zend_parse_parameters_none() == FAILURE) {
        return;
    }

    GET_PROP(items, "items");

    RETVAL_DOUBLE(Z_DVAL(php_collection_sum(items)));
}

METHOD(avg) {
    zval *items, sum;

    if (zend_parse_parameters_none() == FAILURE) {
        return;
    }

    GET_PROP(items, "items");

    ZVAL_DOUBLE(&sum, Z_DVAL(php_collection_sum(items)));

    RETVAL_DOUBLE(Z_DVAL(sum) / php_collection_count(items));
}

METHOD(count) {
    zval *items;

    if (zend_parse_parameters_none() == FAILURE) {
        return;
    }

    GET_PROP(items, "items");

    RETVAL_LONG(php_collection_count(items));
}
