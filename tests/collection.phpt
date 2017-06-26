--TEST--
Test Collection
--FILE--
<?php
print_r(new Collection);
print_r(new Collection(['foo', 'bar']));
--EXPECT--
Collection Object
(
    [items:protected] => Array
        (
        )

)
Collection Object
(
    [items:protected] => Array
        (
            [0] => foo
            [1] => bar
        )

)
