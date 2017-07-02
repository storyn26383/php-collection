--TEST--
Test Collection Filter
--FILE--
<?php
print_r((new Collection([1, 2, 3]))->filter(function ($n) {
    return $n > 1;
}));
--EXPECT--
Array
(
    [1] => 2
    [2] => 3
)
