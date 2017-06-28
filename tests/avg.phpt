--TEST--
Test Collection Avg
--FILE--
<?php
print_r((new Collection([1, 2, 3]))->avg());
--EXPECT--
2
