// FlashPromp.h

#ifndef _FLASHPROMP_h
#define _FLASHPROMP_h

#ifdef __cplusplus
extern "C" {
#endif

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

	extern void memcpy_FlashProm(char* RAM_Malloc_Pointer, const char* FlahProm_Address, uint16_t NumberOfBytes);

#ifdef __cplusplus
}
#endif

#endif

