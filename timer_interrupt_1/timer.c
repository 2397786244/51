//数码管两位显示秒数。0-99
//定时器运行一次50毫秒
#include <reg52.h>

typedef unsigned int uint;
typedef unsigned char uchar;

sbit WE = P2^7;
sbit DU = P2^6;
sbit key_s2 = P3^0; //按下这个键可以把定时器清零。
sbit int_0 = P3^2;

uchar ch[10] = {0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f};
uint sec=0,mSec=0;

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
	EA=1;
	ET0 = 1; //定时器中断打开 IE寄存器
	IT0 = 0; //低电平触发。外部中断0
	TMOD = 0x01; //M1 = 0,M0 = 1即16位全开的定时器模式
	TR0 = 1;  //定时器0打开。 
	EX0 = 1; //外部中断0
	TH0 = 0x4B;
	TL0 = 0xfd; //定时器初始值。到溢出需要50毫秒
}

void keyint()interrupt 0{
	//按下S2的时候，定时器清空为0，重新开始计时
	sec = 0;
	mSec = 0;
	TH0 = 0x4b;
	TL0 = 0xfd;
	while(!key_s2); //松手检测。
}

void timer0()interrupt 1{
	mSec++;  //说明已经经过了50ms
	//把TH0 TL0再次赋值
	TH0 = 0x4b;
	TL0 = 0xfd;
	if(20==mSec){
		mSec = 0;
		sec++;
	}
	if(sec>99)sec = 0;
}
void main(){
	timer0init(); //定时器初始化
	while(1){
		int_0 = 1;
		if(0==key_s2){
			delay(20);
			if(0==key_s2){
				int_0 = 0;
				//给外部中断0一个低电平。	
			}
		}
		display(sec);
	}
}