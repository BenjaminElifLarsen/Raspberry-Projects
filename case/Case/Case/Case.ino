
#include <XBee.h>
#include <Printers.h>

//#define USE_SERIAL Serial2

#include <Ticker.h>

#include "Definitions.h"
#include "FlashPromp.h"

#include "SoftwareSerial.h"
#include "avr8-stub.h"
#include "app_api.h"

void ChangeState(void);
void PermitRead(void);

 int TIMER_CHANGE_STATE_MS = 10000;
 int TIMER_HC_DELAY_MS = 2000;
Ticker timerTemp(ChangeState, TIMER_CHANGE_STATE_MS,0);
Ticker timerRead(PermitRead, TIMER_HC_DELAY_MS, 1);
volatile bool TimerTempActive = false;

volatile bool CanReadHC;

static volatile MachineStates State = IDLE;
MachineStateStruct* MachineStateInRAM_Pointer;

volatile bool HC_LED = 53;
volatile bool TEMP_LED = 52;
volatile bool TRANSMIT_LED = 51;

int tempPin = 15;
volatile float temperature = 0;
 
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
	//USE_SERIAL.write(c);
}

void IdleState(void)
{
	State = IDLE;
}

void TimerUpdates(void)
{
	timerRead.update();
	timerTemp.update();
}

void ChangeState(void)
{
	uint8_t stateInt = (int)State;
	stateInt = (++stateInt) % UNKNOWN_STATE;
	State = (MachineStates)stateInt;
}

void PermitRead(void) 
{
	CanReadHC = true;
	timerRead.stop();
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
	while (IDLE == State && true == CanReadHC) 
	{
		TimerUpdates();
		int distance = ReadHCSR04();
		if (10 <= distance && 100 >= distance)
		{
			CanReadHC = false;
			timerRead.start();
			if (!TimerTempActive) {
				timerTemp.start();
				TimerTempActive = true;
			}
			else {
				timerTemp.stop();
				TimerTempActive = false;
			}
			//ChangeState(); //replace with activating a timer on an interrupt. The current call is just for testing.
			//have another time that need to be run ones before it is possible to deactivate the first timer.
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
	State = READING_TEMPERATURE;
	temperature = ReadTemperature();
	ChangeState();
}

void TransmitTemperature(void)
{
	//Xbee call
}

void DataTransmission(void)
{
	TransmitTemperature();
	ChangeState();
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

//void PortManipulation(char* address_string, uint8_t position, uint8_t value)
//{
//	uint8_t* address_Pointer;
//
//	address_Pointer = (uint8_t*)(uint16_t)strtoul((const char*)address_string, NULL, 16);
//
//	if (0 == value)
//	{
//		uint8_t* ddr = address_Pointer;
//		ddr--;
//		*ddr &= ~(1 << position);
//		*address_Pointer &= ~(1 << position);
//	}
//	else
//	{
//		uint8_t* ddr = address_Pointer;
//		ddr--;
//		*ddr |= 1 << position;
//		*address_Pointer |= 1 << position;
//	}
//}

void setup() 
{
	//USE_SERIAL.begin(9600);
	debug_init();
	delay(3000);
	////PortManipulation("25", 3, 1);
	//pinMode(TRIG, OUTPUT); //move over to C code later
	////PortManipulation(ECHO_PORT, ECHO_BIT, 1);
	//pinMode(ECHO, INPUT);
	CanReadHC = true;
}

void loop() 
{
	TimerUpdates();
	MachineStateInRAM_Pointer = StateAllocateMemoryInRamAndGetCopyFromFlashProm();
	MachineStateInRAM_Pointer->FunctionPointer();
}
