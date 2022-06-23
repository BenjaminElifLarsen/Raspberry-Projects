
#include <XBee.h>
#include <Printers.h>

#define USE_SERIAL Serial2
#define XBEE_SERIAL Serial //could redirect to another Serial

#include <Ticker.h>

#include "Definitions.h"
#include "FlashPromp.h"

#include "SoftwareSerial.h"
#include "avr8-stub.h"
#include "app_api.h"

XBee xbee = XBee();

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

#define IDLE_LED 53
#define TEMPERATURE_LED 52
#define TRANSMITTING_LED 51

#define ADDRESS_HIGH 0x0013A200
#define ADDRESS_LOW 0x4155b982

static volatile bool EndOfStateMachine = false;

int my_putc(char c, FILE* t)
{
	USE_SERIAL.write(c);
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
	LedSwitch(IDLE_LED, 1);
	while (IDLE == State && true == CanReadHC) 
	{
		TimerUpdates();
		int distance = ReadHCSR04();
		if (RANGE_MIN <= distance && RANGE_MAX >= distance)
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
		}
	}
	LedSwitch(IDLE_LED, 0);
}

float ReadTemperature(void) 
{
	int val = analogRead(tempPin);
	float mv = (val / 1024.0) * 5000;
	return mv / 10;
}

void TemperatureHandling(void)
{
	LedSwitch(TEMPERATURE_LED, 1);
	State = READING_TEMPERATURE;
	temperature = ReadTemperature();
	ChangeState();
	LedSwitch(TEMPERATURE_LED, 0);
}

void TransmitTemperature(void)
{
	//Xbee call
	XBeeAddress64 addr64 = XBeeAddress64(ADDRESS_HIGH,ADDRESS_LOW);

	char tempChar[7];
	dtostrf(temperature, 4, 4, tempChar);

	uint8_t payload[7];
	for (int i = 0; i < sizeof(payload); i++) {
		payload[i] = tempChar[i];
	}
	ZBTxRequest zbTx = ZBTxRequest(addr64,payload, sizeof(payload));
	xbee.send(zbTx);
	if (!xbee.readPacket(2500)) {

		//how to handle the error
		uint8_t error = xbee.getResponse().getErrorCode();
		printf("Error: " + error);
	}

}

void DataTransmission(void)
{
	LedSwitch(TRANSMITTING_LED, 1);
	TransmitTemperature();
	ChangeState();
	LedSwitch(TRANSMITTING_LED, 0);
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

void LedSwitch(int pin, bool on) 
{
	digitalWrite(pin, on);
}

void setup() 
{
	XBEE_SERIAL.begin(9600);
	USE_SERIAL.begin(9600);
	debug_init();
	delay(3000);
	pinMode(IDLE_LED, 1);
	pinMode(TEMPERATURE_LED, 1);
	pinMode(TRANSMITTING_LED, 1);
	////PortManipulation("25", 3, 1);
	//pinMode(TRIG, OUTPUT); //move over to C code later
	////PortManipulation(ECHO_PORT, ECHO_BIT, 1);
	//pinMode(ECHO, INPUT);
	CanReadHC = true;
	fdevopen(&my_putc, 0);
	xbee.setSerial(XBEE_SERIAL);
}

void loop() 
{
	TimerUpdates();
	MachineStateInRAM_Pointer = StateAllocateMemoryInRamAndGetCopyFromFlashProm();
	MachineStateInRAM_Pointer->FunctionPointer();
}
