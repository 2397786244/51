/**
*@brief
* 定时器延时
* 从P1.0IO口输入P3.4口脉冲信号，使计数器增加
* 在数码管上显示数值。
* 把P3.5口接P1.0
***/
#include<reg52.h>

typedef unsigned int uint;
typedef unsigned char uchar;
uint mSec=0;
uint sec=0;
uchar ch[10] = {0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f};

sbit DU = P2^6;
sbit WE = P2^7;
sbit LED1 = P1^0;

void delay(uint n){
	uint i,j;
	for(i=0;i<n;i++)
		for(j=0;j<114;j++);
}

void display(uint num){
	uint shi;
	uint ge;
	shi = num / 10;
	ge = num % 10;
	
	P0 = 0xff;
	WE = 1;
	P0 = ~0x01;
	WE = 0;
	
	DU = 1;
	P0 = ch[shi];
	DU = 0;
	delay(10);
	
	P0 = 0xff;
	WE= 1;
	P0 = ~0x02;
	WE = 0;
	
	DU = 1;
	P0 = ch[ge];
	DU = 0;
	delay(10);
}

void timer0init(){
	//timer0实现定时器的功能。（硬件延时）
	TR0 = 1;
	TMOD |= 0x01; 
	TH0 = 0x4b;
	TL0 = 0xd;
}
void timer1init(){
	TR1 = 1; //定时计数器1作为计数器功能
	TMOD |= 0x50;
	TH1 = 0;
	TL1 = 0; //低八位
}

void main(){
	timer0init();
	timer1init();//对两个定时计数器初始化。
	while(1)
	{
		if(TF0)
		{
			//timer0已经溢出
			mSec++; //过去了50ms
			TH0 = 0x4b;
			TL0 = 0xfd;
			TF0 = 0;
			if(20==mSec)
			{
				//过去了1000ms
				sec++;
				mSec = 0;
				//用LED1发出一个脉冲
				LED1 = ~LED1;
				LED1 = ~LED1; //但是这样会导致看不到LED灯亮。
				//可以把if条件改为(10==mSec)

			}
			//以上代码使得计数器能够每1000ms增加1
		}
		if(TL1>99)TL1 = 0;
		display(TL1);
	}
}
