
#include <XBee.h>
#include <Printers.h>

#define USE_SERIAL Serial2

#include "Definitions.h"
#include "FlashPromp.h"

#include "SoftwareSerial.h"
#include "avr8-stub.h"
#include "app_api.h"


static volatile MachineStates State = IDLE;
static volatile MachineStateStruct WorkingStruct;

#define TEMP_PIN 15;

static volatile bool EndOfStateMachine = false;

int my_putc(char c, FILE* t)
{
	USE_SERIAL.write(c);
}

void IdleState(void)
{
	State = IDLE;
}

double ReadHCSR04(void)
{

}

void DistanceHandling(void)
{
	ReadHCSR04();
}

float ReadTemperature(void) 
{
	int val = analogRead(TEMP_PIN);
	float mv = (val / 1024.0) * 5000;
	return mv / 10;
}

void TemperatureHandling(void)
{
	ReadTemperature();
}

void TransmitTemperature(void)
{
	
}

void DataTransmission(void)
{
	TransmitTemperature();
}

const MachineStateStruct PROGMEM MachineStateStructArray[] =
{
	{IDLE, DistanceHandling, "0x0025", 2},
	{READING_TEMPERATURE, TemperatureHandling, "0x0025", 1},
	{TRANSMITTING_DATA, DataTransmission, "0x0025", 0},
};

MachineStateStruct* StateAllocateMemoryInRamAndGetCopyFromFlashProm()
{
	MachineStateStruct* MachineStructInRAM_Pointer = (MachineStateStruct*)malloc(sizeof(MachineStateStruct));
	memcpy_FlashProm((char*)MachineStructInRAM_Pointer, (const char*)&(MachineStateStructArray[(uint8_t)State]), sizeof(MachineStateStruct));
}

void setup() {
	USE_SERIAL.begin(9600);
	debug_init();
	delay(3000);
}

void loop() {
	USE_SERIAL.print("\nTest");
}
