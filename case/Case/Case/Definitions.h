#pragma once


typedef void (*Function_Pointer)(char*);

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
	uint8_t LEDPort;
	char LEDPort_String[5];
	uint8_t LEDBitLocation;
} MachineStateStruct;