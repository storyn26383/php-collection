--TEST--
Test Collection Map
--FILE--
<?php
print_r((new Collection([1, 2, 3]))->map(function ($n) {
    return $n * 2;
}));
--EXPECT--
Array
(
    [0] => 2
    [1] => 4
    [2] => 6
)
