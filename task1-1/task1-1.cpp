#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <wiringSerial.h>
#include <wiringPi.h>
#include <iostream>
using namespace std;
#define LED 0

int main(void)
{
 wiringPiSetup();
 pinMode(LED,OUTPUT);
 int fd;
 if((fd = serialOpen("/dev/ttyUSB0",9600)) < 0)
 {
  fprintf(stderr, "unable to open serial device: %s\n", strerror(errno));
  return 1;
 }
 
 for(;;)
 {
  char chr = serialGetchar(fd);
  putchar(chr);
  cout << chr;
  if(chr == '0')
  {
   digitalWrite(LED,LOW);
  }
  else if(chr == '1')
  {
   digitalWrite(LED,HIGH);
  }

  fflush(stdout);
 }
 //string s;
 //cin >> s;
 //cout << s;
 return 0;
}
