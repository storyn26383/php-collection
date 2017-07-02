#include "php_collection.h"

#if COMPILE_DL_COLLECTION
    ZEND_GET_MODULE(collection)
#endif

ZEND_BEGIN_ARG_INFO_EX(arginfo_construct, 0, 0, 0)
    ZEND_ARG_ARRAY_INFO(0, items, 1)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_map, 0, 0, 1)
    ZEND_ARG_CALLABLE_INFO(0, callback, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_reduce, 0, 0, 2)
    ZEND_ARG_CALLABLE_INFO(0, callback, 0)
    ZEND_ARG_INFO(0, default_value)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_filter, 0, 0, 1)
    ZEND_ARG_CALLABLE_INFO(0, callback, 0)
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
        PHP_ME(Collection, map, arginfo_map, ZEND_ACC_PUBLIC)
        PHP_ME(Collection, reduce, arginfo_reduce, ZEND_ACC_PUBLIC)
        PHP_ME(Collection, filter, arginfo_filter, ZEND_ACC_PUBLIC)
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

#define INIT_CALLBACK(callback, args, arg_count, return_value) do { \
    callback.params = args; \
    callback.param_count = arg_count; \
    callback.retval = return_value; \
    callback.no_separation = 0; \
} while (0)

#define UPDATE_HASH(array, str_key, num_key, value) do { \
    if (str_key) { \
        zend_hash_update(array, str_key, value); \
    } else { \
        zend_hash_index_update(array, num_key, value); \
    } \
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

    // TODO: not working, why?
    /* ZEND_PARSE_PARAMETERS_START(0, 1) */
    /*     Z_PARAM_OPTIONAL */
    /*     Z_PARAM_ARRAY(items) */
    /* ZEND_PARSE_PARAMETERS_END(); */

    /* RETVAL_NULL(); */

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "|a", &items) == FAILURE) {
        RETVAL_NULL();
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

METHOD(map) {
    zval *items, *value, result, arg;
    zend_fcall_info callback;
    zend_fcall_info_cache callback_cache = empty_fcall_info_cache;
    zend_ulong num_key;
    zend_string *str_key;

    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_FUNC(callback, callback_cache)
    ZEND_PARSE_PARAMETERS_END();

    GET_PROP(items, "items");

    array_init(return_value);

    INIT_CALLBACK(callback, &arg, 1, &result);

    ZEND_HASH_FOREACH_KEY_VAL(Z_ARRVAL_P(items), num_key, str_key, value) {
        ZVAL_COPY(&arg, value);

        zval_ptr_dtor(&arg);

        if (zend_call_function(&callback, &callback_cache) != SUCCESS || Z_TYPE(result) == IS_UNDEF) {
            zend_array_destroy(Z_ARR_P(return_value));
            RETURN_NULL();
        }

        UPDATE_HASH(Z_ARRVAL_P(return_value), str_key, num_key,  &result);
    } ZEND_HASH_FOREACH_END();
}

METHOD(reduce) {
    zval *items, *default_value, *value, retval, *entry, result;
    zval args[2];
    zend_fcall_info callback;
    zend_fcall_info_cache callback_cache = empty_fcall_info_cache;
    int success;

    ZEND_PARSE_PARAMETERS_START(2, 2)
        Z_PARAM_FUNC(callback, callback_cache)
        Z_PARAM_ZVAL(default_value)
    ZEND_PARSE_PARAMETERS_END();

    GET_PROP(items, "items");

    INIT_CALLBACK(callback, args, 2, &retval);

    ZEND_HASH_FOREACH_VAL(Z_ARRVAL_P(items), entry) {
        ZVAL_COPY(&args[0], &result);
        ZVAL_COPY(&args[1], entry);

        success = zend_call_function(&callback, &callback_cache) == SUCCESS && Z_TYPE(retval) != IS_UNDEF;

        zval_ptr_dtor(&args[1]);
        zval_ptr_dtor(&args[0]);

        if (!success) {
            return;
        }

        zval_ptr_dtor(&result);
        ZVAL_COPY_VALUE(&result, &retval);
    } ZEND_HASH_FOREACH_END();

    RETVAL_ZVAL(&result, 1, 1);
}

METHOD(filter) {
    zval *items, *value, result, entry;
    zend_fcall_info callback;
    zend_fcall_info_cache callback_cache = empty_fcall_info_cache;
    zend_ulong num_key;
    zend_string *str_key;
    int success;

    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_FUNC(callback, callback_cache)
    ZEND_PARSE_PARAMETERS_END();

    GET_PROP(items, "items");

    array_init(return_value);

    INIT_CALLBACK(callback, &entry, 1, &result);

    ZEND_HASH_FOREACH_KEY_VAL(Z_ARRVAL_P(items), num_key, str_key, value) {
        ZVAL_COPY(&entry, value);

        zval_ptr_dtor(value);

        success = zend_call_function(&callback, &callback_cache) == SUCCESS;

        if (success && !Z_ISUNDEF(result) && zend_is_true(&result)) {
            UPDATE_HASH(Z_ARRVAL_P(return_value), str_key, num_key,  &entry);
        }

        zval_ptr_dtor(&result);

        if (!success) {
            return;
        }
    } ZEND_HASH_FOREACH_END();
}
