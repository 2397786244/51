/** PWM 定频调宽 用两个按键控制LED灯的亮度**/
#include <reg52.h>
#include <intrins.h>

typedef unsigned int uint;
typedef unsigned char uchar;

sfr P4 = 0xe8;
//位定义
sbit S2 = P3^0;
sbit S3 = P3^1;

sbit DOUT = P2^5;
sbit CS = P3^7; //片选信号。
sbit DIN = P2^0;
sbit DCLK = P2^1;
sbit DA_DATA = P4^4;

uchar DA_VAL = 80;  //0-255取值，控制PWM的占空比。80是能让LED点亮的最小值
//DA_VAL / 255 * 5V = voltage
uchar pwmCnt = 0; //到255之后溢出置0，用于定时器0中断函数

void delay(uint n){
	uint i,j;
	for(i=0;i<n;i++)
		for(j=0;j<114;j++);
}

void timer0Init(){
	EA = 1;
	ET0 = 1;
	TMOD = 0x01;
	TL0 = 0xdc;  //100hz
	TH0 = 0xff;
	TR0 = 1;
}

void pwmGenerate() interrupt 1{
	pwmCnt++;
	if(pwmCnt<=DA_VAL){
		DA_DATA = 1; //一个周期内为高电平的情况。DA_VAL控制宽度
	}	
	else{
		DA_DATA = 0;
	}
	TL0 = 0xdc;
	TH0 = 0xff; //执行36次之后就会溢出。 100hz左右
}	

void main(){
	timer0Init();
	while(1){ //按s2减少led亮度，按s3增加亮度。
		if(0 == S2){
			delay(10);
			//防抖
			if((0 == S2) && DA_VAL > 80){
				DA_VAL--;
				pwmCnt = 0;
				TL0 = 0xdc;
				TH0 = 0xff;
			}
			//while(!S2); 取消松手检测，按着会一直调整亮度。
		}
		if(0 == S3){
			delay(10);
			//防抖
			if((0 == S3) && DA_VAL < 255){
				DA_VAL++;	
				pwmCnt = 0;
				TL0 = 0xdc;
				TH0 = 0xff;
			}
			//while(!S3);
		}
	}
}	