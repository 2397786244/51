#include<reg52.h>
#include<intrins.h>

#define CMD_AIN1 0x94 
#define CMD_AIN2 0xd4
#define CMD_AIN4 0xa4
#define CMD_AIN3 0xe4
//光敏AIN1 热敏AIN2 电位器AIN4 外部输入AIN3
typedef unsigned int uint;
typedef unsigned char uchar;

sbit WE = P2^7;
sbit DU = P2^6;
sbit DIN = P2^0; //串行数据输入端
sbit DCLK = P2^1;  //提供时钟线
sbit CS = P3^7;
sbit DOUT = P2^5; //串行输出
uchar code ch[10] = {0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f};
uchar code pos[4] = {0xfe, 0xfd, 0xfb,0xf7};
uint wei = 0;

//延时函数
void Delay_Ms(uint ms)
{
     uint i;
	 do{
	      i = 11059200UL / 96000; 
		  while(--i);   //96T per loop
     }while(--ms);
}

void display(uint i)
{
	uchar qian, bai, shi, ge;
	static uchar wei;
	qian = i / 1000;
	bai = i % 1000 / 100;
	shi = i % 100 / 10;
	ge = i % 10;
	
	P0 = 0XFF;//清除断码
	WE = 1;//打开位选锁存器
	P0 = pos[wei];
	WE = 0;//锁存位选数据
	P0 = 0XFF;//清除断码
	switch(wei)
	{
		case 0: DU = 1; P0 = ch[qian] | 0x80;	DU = 0; break;//0x80加上小数点
		case 1: DU = 1; P0 = ch[bai]; 	DU = 0; break;	
		case 2: DU = 1; P0 = ch[shi]; 	DU = 0; break;
		case 3: DU = 1; P0 = ch[ge]; 	DU = 0; break;		
	}
	wei++;
	if(wei == 4)
		wei = 0;
}

void spiSend(uchar cmd){
	uint i;
	for(i=0;i<8;i++){
		DCLK = 0;
		if(cmd & 0x80){
			DIN = 1;
		}	
		else
			DIN = 0;
		DCLK = 1;//上升沿的时候锁存一位数据
		cmd <<= 1;
	}	
}

uint spiRead(){
	uint i;
	uint dat = 0;
	for(i = 0;i<12 ;i++){
		//选择XPT2604 12位模式
		dat <<= 1;
		DCLK = 1;
		DCLK = 0; //下降沿的时候从DOUT上读一位数据。
		if(DOUT)
			dat |= 0x01;
	}
	return dat;
}	

uint adTrans(uchar cmd){
	uint dat;
	CS = 0; //片选信号先置0
	spiSend(cmd); //发送控制指令
	DCLK = 0; //发完指令后先把DCLK拉低
	_nop_();
	_nop_();
	_nop_();
	_nop_();
	_nop_();
	//延时25us左右。
	//延时后，可以从DOUT上读数据
	dat = spiRead();
	CS = 1; //转换结束。把CS拉高
	return dat;
}

void main(){
	uint msCnt;
	uint value;
	while(1){
		if(100 <= msCnt)
		{
			msCnt = 0;
			value = adTrans(CMD_AIN1); //每500ms进行一次转换。
			value *= 1.2207; //5000mv/4096的转换 value实际上得到的是0-4095之间的数码
		}
		msCnt++;
		display(value);
		Delay_Ms(5);
	}	
}	