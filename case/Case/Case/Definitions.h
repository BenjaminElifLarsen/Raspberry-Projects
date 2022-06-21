#pragma once


typedef void (*Function_Pointer)(void);

typedef enum
{
	IDLE,
	READING_TEMPERATURE,
	TRANSMITTING_DATA,
	UNKNOWN_STATE,
} MachineStates;

typedef struct {
	MachineStates MachineState;
	Function_Pointer FunctionPointer;
	char LEDPort_String[7];
	uint8_t LEDBitLocation;
} MachineStateStruct;