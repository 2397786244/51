/**
* 串口通信。单片机接收数字，数码管显示。独立键盘按下S2累加value发送至电脑
*/

#include <reg52.h>

typedef unsigned int uint;
typedef unsigned char uchar;

uchar code table[10]={0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f};
uint code pos[3] = {0xfe,0xfd,0xfb};
uint value=0;
uint num=0;

sbit WE = P2^7;
sbit DU = P2^6;
sbit key_s2 = P3^0;

void delay(uint n);
void duli_key();
void display(uint num);

//定时器0和定时器1和串口通信初始化
void timerInit(){
	EA = 1;//总控制位
	ET0 = 1;
	ET1 = 1;//定时器0，1的中断打开
	ES = 1; //串口中断打开
	TR0 = 1;
	TR1 = 1;//启动0和1两个定时器
	//给定时器0和1赋初值
	//定时器1用于串口通信。模式为8位自动重装
	TMOD = 0x21; //定时器1的设置为0x20;
	TL0 = 0xff;
	TH0 = 0xed;
	TL1 = 0xfd;
	TH1 = 0xfd;//9600波特率
	
	//串口中断的设置
	REN = 1;//接收控制位
	SM0 = 0;
	SM1 = 1;
}
//延时函数。矩阵键盘的扫描
void delay(uint n){
	uint i,j;
	for(i=0;i<n;i++)
		for(j=0;j<114;j++);
}

//数码管显示
void display(uint num){
	static uint wei=0;
	uint bai;
	uint shi;
	uint ge;
	bai = num /100;
	shi = num/10 % 10;
	ge = num % 10;
	
	//确定位选
	P0 = 0xff;
	WE = 1;
	P0 = pos[wei];
	WE = 0;
	switch(wei){
		case 0:DU = 1;P0 = table[bai];DU = 0;break;
		case 1:DU = 1;P0 = table[shi];DU = 0;break;
		case 2:DU = 1;P0 = table[ge];DU = 0;break;
	}
	wei++;
	if(wei==3)
		wei = 0;
}

//定时器0的中断函数.用于display的延时显示
void timer0() interrupt 1{
	//把TL0和TH0重新赋值为0xff和0xed;
	TH0 = 0xed;
	TL0 = 0xff;
	display(num);
}

//串口中断函数
void uart() interrupt 4{
	//先判断TI是否为1.是否请求中断
	if(TI){
		TI = 0;
	}
	if(RI){
		//接收中断标志位
		num = SBUF;
		RI = 0;
	}
}
//独立键盘
void duli_key(){
	//key_s2一开始是1
	if(!key_s2){
		delay(10);
		if(!key_s2){
			value++;
			SBUF = value;
		}
		while(!key_s2);
	}
}

void main(){
	timerInit();
	while(1){
		duli_key();
	}
}
