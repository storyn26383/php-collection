PHP_ARG_ENABLE(collection, whether to enable collection extension support,
  [--enable-collection Enable collection extension support])

if test $PHP_COLLECTION != "no"; then
    PHP_NEW_EXTENSION(collection, php_collection.c, $ext_shared)
fi
