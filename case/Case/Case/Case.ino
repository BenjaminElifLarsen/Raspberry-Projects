
#include <XBee.h>
#include <Printers.h>
#define USE_SERIAL Serial3

#include "Definitions.h"

#include "SoftwareSerial.h"
#include "avr8-stub.h"
#include "app_api.h"

static volatile MachineStates State = IDLE;
static volatile MachineStateStruct WorkingStruct;

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

double ReadTemperature(void) 
{

}

void TemperatureHandling(void)
{
	ReadTemperature();
}

void TransmitTemperature(void)
{

}

void setup() {
}

void loop() {
}
