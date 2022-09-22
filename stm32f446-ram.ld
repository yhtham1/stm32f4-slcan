/*
  run from RAM
*/

MEMORY
{
	rom (rx) : ORIGIN =  0x20000000, LENGTH = 64K
	ram (rwx) : ORIGIN = 0x20010000, LENGTH = 64K
}

INCLUDE cortex-m-generic.ld 



