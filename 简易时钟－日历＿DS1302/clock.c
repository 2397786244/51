#include <reg52.h>
#include <stdio.h>
//DS1302写寄存器 起始地址
#define REG_W_START 0x80
//DS1302 读寄存器 起始地址
#define REG_R_START 0x81

/*
使用LCD1602显示当前的时间
按下按键S2，显示当前的年月日信息(星期）。
使用ds1302 RAM储存信息。
按下S3，发送时间到串口。
*/

//DS1302三个引脚使用P1.0 1.1 1.2，用杜邦线提前连接
sbit SCLK = P1^0;
sbit IO = P1^1;
sbit CS = P1^2;

//LCD 1602 引脚
sbit LCDEN = P3^4;
sbit RS = P3^5; //数据，命令选择
sbit RW = P3^6; //数据读写。

//按键
sbit S2 = P3^0;
sbit S3 = P3^1;

typedef unsigned char uchar;
typedef unsigned int uint;

//LCD1602 函数部分 命令发送，数据发送，指针位置设置，显示屏初始化
uchar TIME_AR[7] = {
20,50,5,14,7,1,22
};

//设置LCD显示状态
enum STATES {CLOCK,Y_M_D};
static enum STATES S = CLOCK;

void sendTimeToSeries();
//延时函数
void delay(uchar time){
	uint i,j;
	for(i = 0;i<114;i++){
		for(j = 0;j < time;j++);
	}
}
//判断是否空闲
void readStates(){
	uchar flag = 0;
	P0 = 0xff; //把P0寄存器（即D0-D7）拉高。
	RS = 0;  //指令选择
	RW = 1;  //读选择
	do{
		LCDEN = 1;  //使能
		flag = P0;  //把数据口赋值给flag，判断flag第八位(D7)是否为0，为0则空闲退出循环。
		LCDEN = 0;
	}while(flag & 0x80);
}

//发送命令
void SendCmd(uchar cmd){
	 //每次发出指令前判断器件是否空闲
	readStates();
	RS = 0;
	RW = 0;
	P0 = cmd;
	LCDEN = 1;
	LCDEN = 0;//指令发送完之后拉低。两次使能信号改变语句之间间隔在时序规定的最大值之内
}

//数据发送
void SendData(uchar dat){
	readStates();
	RS = 1;  //选择数据
	RW = 0;  //写
	P0 = dat; 
	LCDEN = 1;
	LCDEN = 0;//使能保持150ns
}

//指针位置的设置，调用发送命令函数
int setPointer(uchar x,uchar y){
	switch(x){
		case 1: SendCmd(0x80 | y);break;
		case 2:	SendCmd(0x80 | 0x40 | y);break;
	}
	return 1;
}

//显示屏初始化
void lcdInit(){
	SendCmd(0x38);
	SendCmd(0x0c); 
	SendCmd(0x01); //清屏
	SendCmd(0x06); //输入一个之后指针往后移动。
}

//发送字符串到显示屏。调用SendData函数
void SendString(uchar * str){
	while(*str != '\0'){
		SendData(*str++);
	}
}

//DS1302部分 器件初始化，时间获取

//写入一字节
void ds1302SendByte(uchar dat){
	uint i;
	for(i = 0;i<8;i++){
		SCLK = 0;
		IO = dat & 0x01;
		SCLK = 1;
		dat >>= 1;
	}
}

//读一字节
uchar ds1302ReadByte(){
	uint i;
	uchar dat;
	SCLK = 1;
	for(i = 0;i<8;i++){
		SCLK = 0;
		dat >>= 1;		
		if(IO){
			dat |= 0x80;
		}
		SCLK = 1;	
	}
	return dat;
}

//发送数据 DS1302发送数据，先发送命令，再发送数据
void DS1302Send(uchar cmd,uchar dat){
	CS = 0;
	SCLK = 0;
	CS = 1;
	ds1302SendByte(cmd);
	ds1302SendByte(dat);
	SCLK = 1; //拉高时钟线
	CS = 0;
}

//BCD转换为十进制码
uchar bcd2d(uchar bcd){
	uchar c1,c2;
	c1 = bcd / 16;
	c2 = bcd % 16;
	return c1 * 10 + c2;
}

//十进制转换为BCD
uchar d2bcd(uchar d){
	uchar c1,c2;
	c1 = d / 10;
	c2 = d % 10;
	return c1 * 16 + c2; 
}

//读取数据 ,DS1302读取数据，先发送命令，再读取数据
uchar DS1302Read(uchar cmd){
	uchar dat;
	CS = 0;
	SCLK = 0;
	CS = 1;
	ds1302SendByte(cmd);
	dat = ds1302ReadByte();
	SCLK = 1;
	CS = 0;
	return dat;
}

//DS1302写保护,寄存器位于0x8e
//设置写保护，1打开。
void writeOpen(uchar flag){
	DS1302Send(0x8e,flag);
}

//器件初始化。设置日历的初始值。
void DS1302Init(){
	uint i;
	uchar start_addr = REG_W_START;
	writeOpen(0);
	for(i = 0;i<7;i++)
	{
		DS1302Send(start_addr,d2bcd(TIME_AR[i]));
		start_addr+=2;
	}
	writeOpen(1);
}

//寄存器时间读取,结果储存在数组TIME_AR中。
void timeGetAndStore()
{
	uint i;
	uchar start_addr = REG_R_START;
	for(i = 0;i<7;i++){
		TIME_AR[i] = bcd2d(DS1302Read(start_addr));
		start_addr+=2;
	}
}

void numberSend(uchar dat){
	uchar c1 = dat / 10;
	uchar c2 = dat % 10;
	SendData(c1 + '0');
	SendData(c2 + '0');
}
//让时间显示在LCD上，显示小时，分钟，秒
void timeDisplay_HMS(){
	//读取TIME_AR0-2并显示 
	uchar TIME_MSG_LINE1[16] = "CURRENT TIME:";
	timeGetAndStore();
	setPointer(1,0);
	SendString(TIME_MSG_LINE1);
	setPointer(2,0);
	SendString("TIME:");
	numberSend(TIME_AR[2]);
	SendData(':');
	numberSend(TIME_AR[1]);
	SendData(':');
	numberSend(TIME_AR[0]);
	SendData(' ');
}

//时间显示，年，月，日，周
void timeDisplay_YMD(){
	uchar TIME_MSG_LINE1[16];
	timeGetAndStore();
	sprintf(TIME_MSG_LINE1,"EASY CALENDAR");
	setPointer(1,0);
	SendString(TIME_MSG_LINE1);
	setPointer(2,0);
	SendString("TIME:");
	numberSend(TIME_AR[6]);
	SendData('-');
	numberSend(TIME_AR[4]);
	SendData('-');
	numberSend(TIME_AR[3]);
	SendData(' ');
}

//按键部分。S2用于切换两种显示，S3记录时间。
void statesAlter(){
	if(!S2){
		delay(10);
		if(!S2){
			switch(S){
				case CLOCK:S = Y_M_D;break;
				case Y_M_D:S = CLOCK;break;
			}
		
		}
		while(!S2); //松手检测
	}
	if(!S3){
		delay(10);
		if(!S3)
		{
			sendTimeToSeries();
			//按下S3，把当前时间发送到串口。
			while(!S3);
		}
	}
}

void timerInit(){
	TR1 = 1; //定时器1工作。
	TMOD |= 0x20;
	TH1 = 0xfd;
	TL1 = 0xfd; //波特率9600
	SM0 = 0;
	SM1 = 1; //串口选择工作模式
}

void sendTimeToSeries()
{
	/*
	串口发送
	*/
}

void main()
{
	uint cnt = 0;
	lcdInit();
	DS1302Init();
	timerInit();
	writeOpen(0);
	while(1){
		cnt++;
		if(cnt >= 3000)
		{
			writeOpen(0);
			switch(S)
			{
				case CLOCK: timeDisplay_HMS();break;
				case Y_M_D: timeDisplay_YMD();break;
			}
			cnt = 0;
			writeOpen(1);
		}
		statesAlter();
	}
}