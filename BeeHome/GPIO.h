#pragma once

#include <IRremoteESP8266.h>
#include <IRrecv.h>
#include <IRutils.h>


#define Outlet3		1
#define Fan1		2

#define NameDevice	Outlet3
//#define NameDevice	Fan1

#define isState(inState, state)	inState==state? 1:0

struct event_01 {
	bool enable;
	bool instate1;
	int day;
	int hour;
	int minute;
	int state1;
	int delay;
	int state2;
	unsigned long time;
	String s;
};

struct sEvent {
	event_01 eIF;
};

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
	#define numRq	3

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
	

	int in[] = { btn1 ,btn2, btn3 };
	button but[numIn] = { but1, but2, but3 };
	//int gpio[] = { GPIO1, GPIO2, GPIO3 };
	output out[numOut] = { out1, out2, out3 };
	int ledblk = Blue;
	sEvent _event[numRq];
	#define ReadIO(pin)	digitalRead(pin)

#elif NameDevice == Fan1

	#define numIn  2
	#define numOut  7
	#define numRq	3

	#define SS1 analogRead(A0)>300 ? 0:1
	#define SS2 digitalRead(3)  //RX pin

	#define LED_1H  1
	#define LED_3H  0
	#define LED_6H  16
	#define LED_LOW 15
	#define LED_MED 12
	#define LED_HI  13
	#define MSO     14
	#define LED_gre 4

	#define IR_in   5

	#define	buzz	2

	#define GPIO1 LED_LOW
	#define GPIO2 LED_MED
	#define GPIO3 LED_HI
	#define GPIO4 MSO
	#define Blue LED_gre
	#define btn1 A0
	#define btn2 3

	button but1 = { btn1, 0, false ,true };
	button but2 = { btn2, 0, true ,true };

	struct output_simp
	{
		int pin;
		int stt;
		unsigned long indelay;
		bool state;
	};

	output_simp outLOW = { LED_LOW, 1, 0, true };
	output_simp outMED = { LED_MED, 2, 0, true };
	output_simp outHIG = { LED_HI, 3, 0, true };
	output_simp out1H = { LED_1H, 1, 0, false };
	output_simp out3H = { LED_3H, 2, 0, false };
	output_simp out6H = { LED_6H, 3, 0, false };

	output_simp outbuzz = { buzz, 0, 0, false };

	output_simp outsped[] = { outLOW , outMED , outHIG };

	output outPower = { -1, 0, 0, 0, 0, false, true };
	output outSpeed = { -1, 0, 0, 0, 0, false, true };
	output outRotate = { MSO, 0, 0, 0, 0, false, true };

	int in[] = { btn1 ,btn2 };
	button but[numIn] = { but1 , but2 };

	//int gpio[] = { GPIO1, GPIO2, GPIO3, GPIO4 };
	output out[numRq] = { outPower, outSpeed, outRotate };
	int ledblk = Blue;

	const uint16_t kRecvPin = IR_in;
	IRrecv irrecv(kRecvPin);

	decode_results results_IR;

	unsigned long key_value = 0;
	sEvent _event[numRq];
	#define ReadIO(pin)	(((pin) == (btn2)) ? (SS2) : (SS1))
	uint _Sped = 0;
	bool _Rota = false;
	ulong Power_indelay = 0;
	bool firston = false;
	bool changedelay = false;
	bool enBuz = false;
	int delay_fiston = 10;
	int delay_changeState = 0;
#else

#error "Define Device Name!"

#endif