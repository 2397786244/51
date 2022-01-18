/**
* 独立按键，矩阵键盘的识别
**/
#include <reg52.h>

typedef unsigned char uchar;
typedef unsigned int uint;

uchar code tabel[10]={0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f}; //0-9

sbit WE = P2^7;
sbit DU = P2^6;

int value=0;

void delay(uint n){
	uint i,j;
	for(i=0;i<n;i++){
		for(j=0;j<114;j++);
	}
}
//延时函数
//键盘扫描
void keyscan()
{
	//矩阵键盘扫描
	P3 = 0xF0;
	if(P3 != 0xF0)
	{
		delay(10); //软件消抖
		if(P3!=0xF0) //进行列扫描
		{
			switch(P3)
				{ //判断是哪一种情况
				case 0XE0: value = 0;break;
				case 0XD0: value = 1;break;
				case 0XB0: value = 2;break;
				case 0X70: value = 3;break;
				}
		P3 = 0x0F; //行扫描
		switch(P3)
			{
			case 0X0E: value += 0;break;
			case 0X0D: value += 4;break;
			case 0X0B: value += 8;break;
			case 0X07: value += 12;break;
			}
		while(P3 != 0X0F); //松手检测。松手前P3肯定不是0x0f;			
		}	
	}
	//下面可以是独立键盘的扫描
	P3 = 0xFF; //给IO口都给高电平。
	if(P3!=0xFF)
	{
		//有按键按下
		switch(P3)
		{
			case 0xFE: value = 0; break;
			case 0xFD: value = 1; break;
			case 0xFB: value = 2; break;
			case 0xF7: value = 3; break;
		}
		while(P3 != 0xFF); //松手检测。
	}
}
void main(){
	WE = 1;
	P0 = 0xFE;
	WE = 0;
	while(1)
	{
		keyscan();
		
		DU = 1;
		P0 = tabel[value];
		DU = 0;
		
	}
}
