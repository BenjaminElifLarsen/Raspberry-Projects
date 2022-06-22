
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

int tempPin = 15;
float temperature = 0;
 
#define ECHO_PORT "2E"
#define ECHO_BIT 4 //bit //PE4 // D2
int ECHO = 2; //only here because of PulseIn function call
#define TRIG_PORT "2E" 
#define TRIG_BIT 3 //bit //PE5 // D3
int TRIG = 3;

#define RANGE_MIN 10
#define RANGE_MAX 100

static volatile bool EndOfStateMachine = false;

int my_putc(char c, FILE* t)
{
	USE_SERIAL.write(c);
}

void IdleState(void)
{
	State = IDLE;
}

int ReadHCSR04(void)
{
	digitalWrite(TRIG, 0); //convert to c 
	//PortManipulation(TRIG_PORT, TRIG_BIT, 0);
	delayMicroseconds(2);
	digitalWrite(TRIG, 1);
	//PortManipulation(TRIG_PORT, TRIG_BIT, 1);
	delayMicroseconds(10);
	digitalWrite(TRIG, 0);
	//PortManipulation(TRIG_PORT, TRIG_BIT, 0);
	long duration = pulseIn(ECHO, 1);
	return duration * 0.034 / 2;
}

void DistanceHandling(void)
{
	while (IDLE == State) 
	{
		int distance = ReadHCSR04();
		if (10 <= distance && 100 >= distance)
		{
			State = READING_TEMPERATURE;
		}
	}
}

float ReadTemperature(void) 
{
	int val = analogRead(tempPin);
	float mv = (val / 1024.0) * 5000;
	return mv / 10;
}

void TemperatureHandling(void)
{
	temperature = ReadTemperature();
	State = TRANSMITTING_DATA;
}

void TransmitTemperature(void)
{
	//Xbee call
}

void DataTransmission(void)
{
	TransmitTemperature();
	State = IDLE;
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

void PortManipulation(char* address_string, uint8_t position, uint8_t value)
{
	uint8_t* address_Pointer;

	address_Pointer = (uint8_t*)(uint16_t)strtoul((const char*)address_string, NULL, 16);

	if (0 == value)
	{
		uint8_t* ddr = address_Pointer;
		ddr--;
		*ddr &= ~(1 << position);
		*address_Pointer &= ~(1 << position);
	}
	else
	{
		uint8_t* ddr = address_Pointer;
		ddr--;
		*ddr |= 1 << position;
		*address_Pointer |= 1 << position;
	}
}

void setup() 
{
	USE_SERIAL.begin(9600);
	debug_init();
	delay(3000);
	//PortManipulation("25", 3, 1);
	pinMode(TRIG, OUTPUT); //move over to C code later
	//PortManipulation(ECHO_PORT, ECHO_BIT, 1);
	pinMode(ECHO, INPUT);
}

void loop() 
{

	//USE_SERIAL.println(ReadTemperature());
	//USE_SERIAL.println(ReadHCSR04());
	//delay(1000);
}
