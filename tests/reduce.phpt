--TEST--
Test Collection Reduce
--FILE--
<?php
print_r((new Collection([1, 2, 3]))->reduce(function ($sum, $n) {
    return $sum + $n;
}, 0));
--EXPECT--
6
