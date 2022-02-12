/** 用光敏电阻调节LED的变化。 先得到光敏电阻电平A-->数字量D，再产生相应的方波，从DOUT输出到LED接口。
**/
#include <reg52.h>
#include <intrins.h> //为_nop_函数提供原型
#define AIN3 0xe4
#define AIN1 0x94
typedef unsigned int uint;
typedef unsigned char uchar;

sbit WE = P2^7;
sbit DU = P2^6;
sfr P4 = 0xe8; //P4寄存器没有在reg52中定义
//位定义
sbit PWM_DATA = P4^4; //输出方波进行放大.从DOUT中输出。
sbit CS = P3^7;
sbit DCLK = P2^1;
sbit DIN = P2^0;
sbit DOUT = P2^5;
uchar code ch[10] = {0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f};
uchar code pos[4] = {0xfe, 0xfd, 0xfb,0xf7};

uint wei = 0;
uchar pwmCnt = 0;
uchar DA_VAL = 80;  //设定pwm占空比。0-255内取值。

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

void display(uint i)
{
	uchar bai, shi, ge;
	static uchar wei;
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
		case 0: DU = 1; P0 = ch[bai]; 	DU = 0; break;	
		case 1: DU = 1; P0 = ch[shi]; 	DU = 0; break;
		case 2: DU = 1; P0 = ch[ge]; 	DU = 0; break;		
	}
	wei++;
	if(wei == 3)
		wei = 0;
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

uint adTrans(uchar cmd){
	uint dat;
	CS = 0; //片选信号置0
	DCLK = 0;
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
	//DA_VAL为全局变量。写在这里会导致中断函数里的DA_VAL始终为0.
	uint msCnt = 0;
	uint value = 0;
	// 80 - 255
	timer0Init();
	while(1){
		if(300 <= msCnt){ //每0.3秒钟获取光敏电阻的电平。
			msCnt = 0;
			EA = 0; //关闭中断，防止定时器中断产生的PWM干扰后面读取DOUT数据。
			value = adTrans(AIN1);
			DA_VAL = value / 16;
			pwmCnt = 0;
			EA = 1;
		}
	display(DA_VAL);
	msCnt++;
	delay(1);
	}
	return 0;
}	