--TEST--
Check for ansiconv presence
--SKIPIF--
<?php if (!extension_loaded("ansiconv")) print "skip"; ?>
--FILE--
<?php 
echo "ansiconv extension is available";
?>
--EXPECT--
ansiconv extension is available
