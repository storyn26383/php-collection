--TEST--
Test Collection Count
--FILE--
<?php
print_r((new Collection([1, 2, 3]))->count());
--EXPECT--
3
