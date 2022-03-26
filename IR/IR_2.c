#include <reg52.h>

typedef unsigned int uint;
typedef unsigned char uchar;

bit IR_DECODE_OK; //红外解码完成
bit IR_OK; //红外接收完成。
uchar IR_DATA[33];  //储存一位引导码，16位用户码和16位键值码
uchar IR_BYTE[4]; //储存红外数据分离出来的四个字节。
uint IR_TIME;

//定时器初始化。外部中断，串口设置
void Init(){
	EA = 1;
	TMOD = 0x22; //八位自动重装模式。
	TL0 = 0;
	TH0 = 0; //定时器0溢出时间为277.76us。
	ET0 = 1; //定时器中断。
	TR0 = 1;//定时器启动。
	
	//外部中断0设置
	IT0 = 1; //选择跳变沿触发方式，引脚从高电平到低电平有效。
	EX0 = 1; //外部中断0允许位。
	
	TL1 = 0xfd;
	TH1 = 0xfd; //设置9600波特率。
	SM0 = 0;
	SM1 = 1;
	TR1 = 1;
}

void time0() interrupt 1{
	IR_TIME++; //每次中断需要256个机器周期。
}

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
	IR_DECODE_OK = 1;
	return 0;
}	
int main(){
	uint i;
	Init();
	while(1){
		if(IR_OK)
		{
			getIRDecode();
			IR_OK = 0;
			if(IR_DECODE_OK)
			{ //解码完成，串口发送结果。
				for(i = 0;i<4;i++){
					SBUF = IR_BYTE[i];
					while(!TI);
					TI = 0;
				}
				IR_DECODE_OK = 0;
			}
		}
	}
	return 0;
}	