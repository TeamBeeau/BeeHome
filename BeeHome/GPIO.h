#pragma once


#define Outlet3		1
#define Fan			2

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
	bool state;
};


#if NameDevice ==  Outlet3	

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
	output out1 = { GPIO1, true };
	output out2 = { GPIO2, true };
	output out3 = { GPIO3, true };
	#define numIn  3
	#define numOut  3

	int in[] = { btn1 ,btn2, btn3, btn4 };
	button but[numIn] = { but1, but2, but3 };

	int gpio[] = { GPIO1, GPIO2, GPIO3, GPIO4 };
	output out[numOut] = { out1, out2, out3 };
	int ledblk = Blue;

#elif NameDevice == Fan
	#define GPIO1 14
	#define GPIO2 12
	#define GPIO3 13
	#define GPIO4 15
	#define Blue 16
	#define btn2 0//12
	#define btn4 5//5
	#define btn3 4////3
	#define btn1 2//0
#else

#error "Define Device Name!"

#endif