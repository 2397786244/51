/** 从P4.4口发出方波，经过放大后输入AIN3口。在数码管上显示这个电压值
**/
#include <reg52.h>
#include <intrins.h> //为_nop_函数提供原型
#define AIN3 0xe4

typedef unsigned int uint;
typedef unsigned char uchar;

sfr P4 = 0xe8; //P4寄存器没有在reg52中定义
//位定义
sbit PWM_DATA = P4^4; //输出方波进行放大.从DOUT中输出。
sbit CS = P3^7;
sbit DCLK = P2^1;
sbit DIN = P2^0;
sbit DOUT = P2^5;

sbit WE = P2^7;
sbit DU = P2^6;

uchar code ch[10] = {0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f};
uchar code pos[4] = {0xfe, 0xfd, 0xfb,0xf7};
uint wei = 0;
uchar pwmCnt = 0;
uchar DA_VAL = 0;  //设定pwm占空比。0-255内取值。

void delay(uint n){
	uint i,j;
	for(i=0;i<n;i++)
		for(j=0;j<114;j++);
}

void timer0Init(){
	EA = 1; //总中断控制
	ET0 = 1; //定时器0中断
	TMOD = 0x02; //八位自动重装
	TL0 = 220;
	TH0 = 220; // 100hz,每次溢出产生的中断让pwm保持一个高电平或输出低电平总共
	// (256 - 220) * 256 * 1.085us 为一个pwm周期。
	TR0 = 1; //定时器打开。
}

void pwmGenerate() interrupt 1{
	if(pwmCnt<=DA_VAL){
		PWM_DATA = 1;
	}	
	else{
		PWM_DATA = 0;
	}
	pwmCnt++; //溢出自动为0
}	

//SPI通信发送控制指令。
void spiSend(uchar cmd){
	uint i;
	for(i=0;i<8;i++){
		DCLK = 0;
		if(cmd & 0x80){
			DIN = 1;
		}
		else{
			DIN = 0;
		}
		DCLK = 1;
		cmd <<= 1;
	}	
}
//从SPI总线上接收数据。
uint spiGet(){
	uint i;
	uint dat = 0;
	for(i=0;i<12;i++){
		dat <<= 1;
		DCLK = 1;
		DCLK = 0; //下降沿之后读取DOUT上一位数据
		if(DOUT)
			dat |= 0x01;
	}	
	return dat;
}

void display(uint i)
{
	uchar qian, bai, shi, ge;
	static uchar wei;
	qian = i / 1000;
	bai = i % 1000 / 100;
	shi = i % 100 / 10;
	ge = i % 10;
	
	P0 = 0XFF;
	WE = 1;
	P0 = pos[wei];
	WE = 0;
	P0 = 0XFF;
	switch(wei)
	{
		case 0: DU = 1; P0 = ch[qian] | 0x80;	DU = 0; break;
		case 1: DU = 1; P0 = ch[bai]; 	DU = 0; break;	
		case 2: DU = 1; P0 = ch[shi]; 	DU = 0; break;
		case 3: DU = 1; P0 = ch[ge]; 	DU = 0; break;		
	}
	wei++;
	if(wei == 4)
		wei = 0;
}
uint adTrans(uchar cmd){
	uint dat;
	CS = 0; //片选信号置0
	spiSend(cmd);
	//发送完控制指令后置DCLK为0
	DCLK = 0;
	_nop_();
	_nop_();
	_nop_();
	_nop_();
	_nop_();
	dat = spiGet();
	CS = 1;
	return dat;
}	
int main()
{
	uint msCnt;
	uint value;
	DA_VAL = 255; //5v电压
	timer0Init();
	while(1){
		if(1000 <= msCnt){
			msCnt = 0;
			EA = 0; //关闭中断，防止定时器中断产生的PWM干扰后面读取DOUT数据。
			value = adTrans(AIN3);
			value = value * 1.22; // 5000mV/4096
			EA = 1;
		}	
	
	msCnt++;
	display(value);
	delay(1);
	}
	return 0;
}	