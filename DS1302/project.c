/*写一个字节数据，读。BCD码
转换 关闭，打开写保护。
设置日历寄存器（初始显示时间），读取时钟日历。 LCD 显示日历*/
#include <reg52.h>
#define READ_START_ADDR 0x81

sbit SCLK = P1^1;
sbit IO = P1^2;
sbit CE = P1^3;
sbit RS = P3^5; //数据，命令端
sbit RW = P3^6; //读，写
sbit EN = P3^4; //使能端
typedef unsigned char uchar;
typedef unsigned int uint;

uchar CALENDAR_DATA[7] = {20,50,5,14,7,1,22};

void sendByte(uchar dat){
	uint i;
	for(i = 0;i<8;i++){
		SCLK = 0;
		IO = dat & 0x01;
		SCLK = 1;
		dat >>= 1;
	}
}

uchar readByte(){
	uint i;
	uchar dat;
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

//一字节数据读取
uchar sRead(uchar cmd){
	uchar dat;
	uint i;
	CE = 0;
	SCLK = 0;
	CE = 1; //上升沿读取数据
	sendByte(cmd);
	dat = readByte();
	return dat;
}

//一字节数据写入
void sWrite(uchar cmd,uchar dat)
{
	uint i;
	CE = 0;
	SCLK = 0;
	CE = 1; //在上升沿开始写入数据
	sendByte(cmd);
	sendByte(dat);
	SCLK = 1; //拉高释放总线
	CE = 0;
}

//bcd还原
uchar bcdToD(uchar bcd){
	uchar dat1,dat2;
	dat2 = bcd  % 16; //个位
	dat1 = bcd / 16; //十位
	return dat1 * 10 + dat2;
}

//转换为BCD码。
uchar dToBCD(uchar dat){
	uchar bcd;
	bcd = dat / 10 * 16 + dat % 10; 
	return bcd;
}

//设置写保护，1打开。
void writeOpen(uint flag){
	if(flag){
		//第八块寄存器设置。
		sWrite(0x8e,1);
	}
	else{
		sWrite(0x8e,0); //关闭写保护
	}
}

void calendarInit(){
	//设置初始时间。写保护关闭。
	//设置时间为2022 7 14  5:50:20
	uchar addr = 0x80;
	uint i;
	writeOpen(0);
	for(i = 0;i<7;i++){
		sWrite(addr,dToBCD(CALENDAR_DATA[i]));
		addr+=2;
	}
	writeOpen(1);
}


//判断液晶屏是否空闲
void readStates(){
	uchar flag = 0;
	P0 = 0xff; //把P0寄存器（即D0-D7）拉高。
	RS = 0;  //指令选择
	RW = 1;  //读选择
	do{
		EN = 1;  //使能
		flag = P0;  //把数据口赋值给flag，判断flag第八位(D7)是否为0，为0则空闲退出循环。
		EN = 0;
	}while(flag & 0x80);
}

void lcdWriteCmd(uchar cmd){
	//每次发出指令前判断器件是否空闲
	readStates();
	RS = 0;
	RW = 0;
	P0 = cmd;
	EN = 1;
	EN = 0;//指令发送完之后拉低。两次使能信号改变语句之间间隔在时序规定的最大值之内
}	

void lcdWriteData(uchar dat){
	readStates();
	RS = 1;  //选择数据
	RW = 0;  //写
	P0 = dat; 
	EN = 1;
	EN = 0;//使能保持150ns
}

void lcdWriteString(uchar dat)
{
	lcdWriteData('0' + dat / 10);
	lcdWriteData('0' + dat % 10);
}

void lcdInit(){
	lcdWriteCmd(0x38);
	lcdWriteCmd(0x0c);
	lcdWriteCmd(0x06);
	lcdWriteCmd(0x01);
}

void calendarRead(){
	uint i;
	uchar addr = READ_START_ADDR;
	for(i = 0;i<7;i++){
		CALENDAR_DATA[i] = bcdToD(sRead(addr));
		addr += 2;
	}
}
void lcdSet(){
	//把年月日，小时，分钟，秒显示在LCD上。
	lcdWriteCmd(0x80);	
	lcdWriteString(CALENDAR_DATA[6]); //year
	lcdWriteCmd(0x82);
	lcdWriteString(CALENDAR_DATA[5]); //week
	lcdWriteCmd(0x84);
	lcdWriteString(CALENDAR_DATA[4]); //month
	lcdWriteCmd(0x86);
	lcdWriteString(CALENDAR_DATA[3]); // day
	lcdWriteCmd(0x80 | 0x40);
	lcdWriteString(CALENDAR_DATA[2]); //hour
	lcdWriteCmd(0x80 | 0x42);
	lcdWriteString(CALENDAR_DATA[1]); //minute
	lcdWriteCmd(0x80 | 0x44);
	lcdWriteString(CALENDAR_DATA[0]);
}

void main(){
	uint i;
	calendarInit();
	lcdInit();
	while(1)
	{
		writeOpen(0);
		calendarRead();
		lcdSet();
		for(i = 0;i<1000;i++);
		writeOpen(1); //时钟开始从2022年7月14日 5点50分20秒开始计时。
	}
}	


