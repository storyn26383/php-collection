--TEST--
Test Collection Sum
--FILE--
<?php
print_r((new Collection([1, 2, 3]))->sum());
--EXPECT--
6
