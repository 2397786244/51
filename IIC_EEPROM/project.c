/**
*模拟IIC总线，EEPROM(带电可擦写只读储存器) 储存发送的数据，电脑接收。
* 发送多个数据，断电后读，用UART检查是不是写入的。
**/
#include<reg52.h>
#include<intrins.h>
#define SADDR 0xA0
typedef unsigned char uchar;
typedef unsigned int uint;

//sbit WE = P2^7;
//sbit DU = P2^6;
sbit SDA = P2^0; //数据线
sbit SCL = P2^1; //IIC 时钟线

//uchar code table[10] = {0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f};
//uchar code pos[] = {0xfe, 0xfd, 0xfb}; 三位数字位选显示的P0口配置
uchar value[16]; //储存EEPROM中读到的数据
uchar datSend[16] = {0x20,0xfe,0xfd,0xfb,0x06,0x66,0x6d,0x7d,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f};
bit ACKstatus = 0;
//从机应答状态。

void delay(uint n); //延时函数
void delay5us(); //延时5us左右。用于IIC模拟通信时序
void iicStart(); //用于IIC发送、接收数据 发出起始信号的函数
void iicEnd(); //总线终止信号函数
bit getACK(); //从从机读应答状态的函数。如果非应答，返回1，否则0
void sendACK(bit i); //主机发送应答状态的函数
uchar getByte(); //从SDA读入数据，并返回给主调函数
void sendByte(uchar dat);  //把参数dat一位一位的发送给SDA。
void iicWrite(uchar addr,uchar dat); //把dat发送给从机(地址为SADDR，写入的首地址为addr，使用sendByte函数)
uchar iicRead(uchar addr); //从地址为SADDR从机的addr处读数据(使用getByte函数)返回给主调函数
//void display(uchar num); 显示value值。在定时器0中断函数中调用

void delay(uint n){
	uint i,j;
	for(i=0;i<n;i++)
		for(j=0;j<114;j++);
}

void delay5us(){
	_nop_();
}

//起始函数.
void iicStart(){
	SCL = 1;
	SDA = 1;
	delay5us(); //至少4.7us
	SDA = 0;
	delay5us(); //至少4us
}
//结束函数。
void iicEnd(){
	SCL = 0;
	SDA = 0;
	SCL = 1;
	delay5us(); //至少4us
	SDA = 1;
	delay5us(); //至少保持4.7us 终止之后把SDA拉高，释放总线
}	

//主机判断是否得到了应答
bit getACK(){
	SCL = 0; //拉低SCL时钟线，允许从机控制SDA
	SCL = 1; //拉高读SDA
	delay5us();
	//保持至少4us后检查SDA
	if(SDA){
		SCL = 0;
		return 1;
	}
	else{
		SCL = 0;
		return 0;
	}	
}

//主机发送应答
void sendACK(bit i){
	SCL = 0; 
	if(i){
		SDA = 1; //NOACK
	}
	else
	{
		SDA = 0;
	}
	SCL = 1;
	delay5us(); //至少保持4us
	SCL = 0;
	SDA = 1; //释放总线
}

//从SDA中读到数据，并返回dat。
uchar getByte(){
	uint i=0;
	uchar dat=0;
	for(i=0;i<8;i++){
		dat <<= 1;
		SCL = 1; //SCL置1，从sda读数据
		if(SDA){
			dat |= 0x01;
		}
		SCL = 0;
	}	
	return dat;
}

//通过IIC总线，往EEPROM写数据。随机写
void iicWrite(uchar addr,uchar dat){
	//首先，发出起始信号
	iicStart();
	//然后发送从机地址加上读写方向。
	sendByte(SADDR + 0);  //0xA0即EEPROM的地址，0是写。
	//然后接收从机的一个应答.
	ACKstatus = getACK();
	//传输要写入的首地址，最大256个字节。
	sendByte(addr);
	ACKstatus = getACK();
	sendByte(dat);
	//发送num到EEPROM中
	ACKstatus = getACK();
	iicEnd();
}

//从EEPROM中读到数据 返回一个值
uchar iicRead(uchar addr){
	uchar dat;
	//首先发出起始信号
	iicStart();
	//发送从机地址
	sendByte(SADDR + 0 ); //从机地址 + 写的方向，传入要读的首地址
	//接收从机一个应答
	ACKstatus = getACK();
	//发送要读的地方的首地址
	sendByte(addr);
	getACK();
	//再次发送起始信号
	iicStart();
	//发送从机地址
	sendByte(SADDR + 1); //读
	ACKstatus = getACK();
	dat = getByte();
	//读完之后发送一个ACK应答。
	sendACK(1);
	iicEnd();
	return dat;
}
//发送字节的的函数。
void sendByte(uchar dat){
	uint i;
	for(i=0;i<8;i++){
		SCL = 0; //把时钟线拉低，允许SDA变化。
		if(dat& 0x80){
			SDA = 1;
		}
		else{
			SDA = 0;
		}
		dat <<= 1; //下一个变为0x40
		SCL = 1; //从机会读SDA数据。
	}
	SCL = 0; //允许SDA变化。
	SDA = 1; //释放数据总线
}
//timer1用于产生9600的波特率
void timerInit(){
	ET1 = 1;
	ES = 1; //串口中断允许位
	TMOD = 0x20; //定时器1用八位自动重装模式
	TH1 = 0xfd;
	TL1 = 0xfd;
	TR1 = 1;
}
void uartInit(){
	REN = 1;//允许接收位 SCON寄存器
	SM0 = 0;
	SM1 = 1;//串口工作方式配置
	//8位UART波特率可变
}
	
//uart发送数据到电脑的函数
void uartSendByte(uchar dat){
	SBUF = dat;
	while(!TI);//等待发送完成。完成之后硬件置1.
	TI = 0; //由软件置0
}	

void main(){
	uint i;
	uchar addr;
	timerInit();
	uartInit();
		EA = 0;
		addr = 0;
		//先往EEPROM写入8个字节的数据
		//写入完成后，断电，重新烧录，从EEPROM中读刚刚写入的数据
		for(i=0;i<16;i++){
			delay(20);	
		    iicWrite(addr,datSend[i]);
			addr++;
		}
		if(ACKstatus){
			P0 = 0;
		}
		else{
			P0 = 0xff;
		}
		addr = 0;
		//从EEPROM读
		for(i=0;i<16;i++){
			delay(20);
			value[i] = iicRead(addr);
			addr++;
		}
		if(ACKstatus){
			P0 = 0;
		}
		else{
			P0 = 0xff;
		}
		EA = 1;
		//最后通过UART发送到电脑
		for(i=0;i<16;i++)
			uartSendByte(value[i]);
	while(1);
}	