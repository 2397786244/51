/*
*流水灯、蜂鸣器
*01/07
*/
#include <reg52.h>
#include <intrins.h> //_crol_ 字符型循环左移 cror 循环右移

typedef unsigned int uint;
typedef unsigned char uchar;
sbit beep = P2^3; //蜂鸣器

void delay(uint n){ //延时函数,软件延时
	uint i,j;
	for(i=0;i<n;i++)
	{
	for(j=0;j<114;j++);
	}
}

void main(){
	uint i;
	//流水灯来回亮的功能
	while(1)
	{
		delay(100);
		for(i=0;i<8;i++)
		{
			beep = ~beep;
			P1 = ~(0x01<<i);  //每次左移一位
			delay(200);
		}
		delay(2000);
	//反方向点亮
		for(i=0;i<8;i++){
			beep = ~beep;
			P1 = ~(0x80>>i); //每次循环右移一位
			delay(200);
		}
		//亮了一个来回之后响一下蜂鸣器
	}
}