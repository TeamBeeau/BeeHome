#pragma once

#include <IRremoteESP8266.h>
#include <IRrecv.h>
#include <IRutils.h>


#define Outlet3		1
#define Fan1		2

#define NameDevice	Outlet3

#define isState(inState, state)	inState==state? 1:0


struct  button
{
	int pin;
	unsigned long time;
	bool pressed;
	bool state;
};

struct output 
{
	int pin;
	int val;
	int temp;
	unsigned long delay;
	unsigned long indelay;
	bool change;
	bool state;
};


#if NameDevice ==  Outlet3	
	#define numIn  3
	#define numOut  3

	#define GPIO1 14
	#define GPIO2 12
	#define GPIO3 13
	#define GPIO4 15
	#define Blue 16
	#define btn2 0//12
	#define btn4 5//5
	#define btn3 4////3
	#define btn1 2//0
	button but1 = { btn1, 0, false ,false };
	button but2 = { btn2, 0, false ,false };
	button but3 = { btn3, 0, false ,false };
	output out1 = { GPIO1, 0, 0, 0, 0, false, true };
	output out2 = { GPIO2, 0, 0, 0, 0, false, true };
	output out3 = { GPIO3, 0, 0, 0, 0, false, true };
	

	int in[] = { btn1 ,btn2, btn3, btn4 };
	button but[numIn] = { but1, but2, but3 };

	int gpio[] = { GPIO1, GPIO2, GPIO3, GPIO4 };
	output out[numOut] = { out1, out2, out3 };
	int ledblk = Blue;

#elif NameDevice == Fan1

	#define numIn  2
	#define numOut  2

	#define SS1 analogRead(A0)>300 ? 0:1
	#define SS2 0//digitalRead(3)  //RX pin

	#define LED_1H  1
	#define LED_3H  0
	#define LED_6H  16
	#define LED_LOW 15
	#define LED_MED 12
	#define LED_HI  13
	#define MSO     14
	#define IR_in   5
	#define LED_gre 4

	#define GPIO1 LED_LOW
	#define GPIO2 LED_MED
	#define GPIO3 LED_HI
	#define GPIO4 MSO
	#define Blue LED_gre
	#define btn1 3
	#define btn2 5

	button but1 = { btn1, 0, false ,false };
	button but2 = { btn2, 0, false ,false };
	output out1 = { GPIO1, 0, 0, 0, 0, false, true };
	output out2 = { GPIO2, 0, 0, 0, 0, false, true };

	int in[] = { btn1 ,btn2 };
	button but[numIn] = { but1, but2 };

	int gpio[] = { GPIO1, GPIO2, GPIO3, GPIO4 };
	output out[numOut] = { out1, out2 };
	int ledblk = Blue;

	const uint16_t kRecvPin = IR_in;
	IRrecv irrecv(kRecvPin);

	decode_results results;

	unsigned long key_value = 0;


#else

#error "Define Device Name!"

#endif