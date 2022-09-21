/*
  run from RAM
*/

MEMORY
{
	rom (rx) : ORIGIN =  0x20000000, LENGTH = 96K
	ram (rwx) : ORIGIN = 0x20018000, LENGTH = 32K
}

INCLUDE cortex-m-generic.ld 



