#include <reg52.h>
/* 遥控板 45 打开电机，47关闭电机 。 40 减速， 44 加速 */
typedef unsigned char uchar;
typedef unsigned int uint;

sbit AIN = P1^0;
//sbit BIN = P1^1;

bit IR_DECODE_OK; //标志红外解码完成
bit IR_OK; //标志接收完成
uchar IR_DATA[33];  //储存一位引导码，16位用户码和16位键值码
uchar IR_BYTE[4]; //储存红外数据分离出来的四个字节。读取第三个字节判断 按键
uint IR_TIME;
uchar pwm;
uchar pwm_length = 120; //最大170
enum STATES {OPEN,OFF};
enum STATES NOW_STATES = OFF; //默认关闭。
int getIRDecode();

//定时器初始化。外部中断，串口设置
void Init(){
	EA = 1;
	TMOD = 0x02; //八位自动重装模式。
	TL0 = 0;
	TH0 = 0; //定时器0溢出时间为277.76us。
	ET0 = 1; //定时器中断。
	TR0 = 1;//定时器启动。
	//外部中断0设置
	IT0 = 1; //选择跳变沿触发方式，引脚从高电平到低电平有效。
	EX0 = 1; //外部中断0允许位。
	TMOD |= 0x20;
	TH1 = TL1 = 220;
	TR1 = 0;
	ET1 = 0;
	//电机初始化。
	AIN = 1;
	//BIN = 1;
}

void time0() interrupt 1{
	IR_TIME++; //每次中断需要256个机器周期。
}

void time1() interrupt 3{
	pwm++;
	if(pwm == 255){
		AIN = 1;
	}
	if(pwm_length == pwm){
		AIN = 0;
	}
}
//接收到红外信号，产生外部中断。
void getIRData() interrupt 0{
	static uint i;
	static bit flag;
	if(flag)
		{
		//判断一开始接收到的是否为起始码。是则从数组首开始存。
		if((IR_TIME >= 32)&& (IR_TIME < 53)) //8.8ms-14.5ms
		{
			i = 0;
		}
		IR_DATA[i] = IR_TIME; //把溢出次数储存。
		i++;
		IR_TIME = 0;
		if(33 == i)
		{
			IR_OK = 1;
			i = 0; //已经储存完33位数据。
			flag = 0; //flag置0准备下一次接收信号。
		}
	}
	else{
		flag = 1;
		IR_TIME = 0;
	}
}	
int getIRDecode(){
	uint i ,j , k = 1; //k是数组下标。跳过引导码。
	for(i = 0;i<4;i++){
		for(j = 0;j<8;j++){
			if(IR_DATA[k] > 5){
				//这位数据为1.
				IR_BYTE[i] |= 0x80;
			}
			if(j < 7)			
				IR_BYTE[i] >>= 1;
			k++;
		}
	}
	return 0;
}	

void IR_Press_Judge(){
	switch(IR_BYTE[2]){
		case 0x45: if(NOW_STATES != OPEN) NOW_STATES = OPEN; TR1 = 1;ET1 = 1;break;
		case 0x47: if(NOW_STATES != OFF) NOW_STATES = OFF; TR1 = 0;ET1 = 0;AIN = 0;break;
		case 0x40: if(pwm_length < 160) pwm_length+=10;break;
		case 0x44: if(pwm_length > 10) pwm_length-=10;break;
	}
}
int main(){
	Init();
	while(1){
		if(IR_OK){
			IR_OK = 0;
			getIRDecode();
			IR_Press_Judge();
		}
	}
}
