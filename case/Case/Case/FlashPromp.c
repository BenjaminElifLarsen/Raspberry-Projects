// 
// 
// 

#include "FlashPromp.h"
#include "Definitions.h"

void memcpy_FlashProm(char* RAM_Malloc_Pointer, const char* FlahProm_Address, uint16_t NumberOfBytes)
{
	memcpy_P(RAM_Malloc_Pointer, FlahProm_Address, NumberOfBytes);
}
