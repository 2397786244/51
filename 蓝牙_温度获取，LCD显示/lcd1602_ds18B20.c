#include "lcd1602_ds18B20.h"
#include <reg52.h>
#include <intrins.h>

sbit RS = P3^5; 
sbit RW = P3^6; 
sbit EN = P3^4; 

sbit DQ = P2^2;  //1-wire总线的数据传输线
void delay(uint n)
{
	uint i,j;
	for(i = 0;i<114;i++)
		for(j=0;j<n;j++);
}

void delay_us(uint us){//进入函数，使用11us，执行一次，使用6us左右。
	while(us>0){
		us--;
	}
}

//LCD1602函数部分
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

void writeCmd(uchar cmd){
	//每次发出指令前判断器件是否空闲
	readStates();
	RS = 0;
	RW = 0;
	P0 = cmd;
	EN = 1;
	EN = 0;//指令发送完之后拉低。两次使能信号改变语句之间间隔在时序规定的最大值之内
}	

void writeData(uchar dat){
	readStates();
	RS = 1;  //选择数据
	RW = 0;  //写
	P0 = dat; 
	EN = 1;
	EN = 0;//使能保持150ns
}

//DS18B20函数部分
bit initCmd(){
	bit flag;
	DQ = 1;
	_nop_(); //一开始为高电平
	DQ = 0;
	delay_us(80);  //保持480us以上
	DQ = 1;
	delay_us(2); //释放总线后保持一段时间，约25us左右，等待器件发出存在信号。
	flag = !DQ;  //如果存在，DQ为0，让函数返回1.	
	delay_us(15);  //保持60us以上
	DQ = 1;
	_nop_();  //不使用总线的时候，都要把它拉高。给延时和开始一样，让他稳定。
	return flag;
}	

void oWireWrite(uchar cmd){
	uint i;
	//拉低总线至少1us，产生一个写时序。
	//如果写入0，拉低总线60-120us,如果写1，在15us之内拉高。两次写入之间隔1us至少。
	for(i = 0;i<8;i++){
		DQ = 0;
		_nop_(); //产生一个写时序。
		if(cmd & 0x01){
			//写1时序
			DQ = 1;  //在15us之内拉高总线
		}	
		else{
			DQ = 0;
			//写0时序
		}
		delay_us(10);
		cmd >>= 1;
		DQ = 1; //释放总线
		_nop_();  //两次写周期的间隔时间。
	}	
}

uchar readByteFromOWire(){
	uchar dat = 0;
	uchar pos = 0x01;
	uint i;
	for(i=0;i<8;i++){
		DQ = 0;
		_nop_();  //拉低总线至少1us。产生读时序。
		DQ = 1; //释放总线
		_nop_();  //让总线电平稳定。
		if(DQ){//采样
			dat = dat | pos; //读到了1 ,另一种用法：dat = (DQ << 7) | (dat >> 1)
		}
		//如果DQ是0就不用管了
		pos <<= 1;
		delay_us(10);  //读完后延时。一次读周期至少要60us以上。
		DQ = 1; //然后释放总线,准备下一次读
		_nop_();  //两次读周期延时
	}
	return dat;
}

void LCD1602Init(){
	//设置显示指令。
	writeCmd(0x38);	
	writeCmd(0x0c);
	//开显示指令
	writeCmd(0x01); //清屏指令
	//数据指针设置
	writeCmd(0x06);
}

void SendString(uchar * s){
	uchar * ptr = s;
	writeCmd(0x80);
	while(*ptr){
		writeData(*ptr);
		ptr++;
	}
}