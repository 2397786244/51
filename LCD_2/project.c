/*算式显示，按键输入*/
#include <reg52.h>
#include <stdlib.h>
#include <stdio.h>

typedef unsigned char uchar;
typedef unsigned int uint;

sbit RS = P3^5; //数据，命令端
sbit RW = P3^6; //读，写
sbit EN = P3^4; //使能端
sbit S2 = P3^0;
sbit S3 = P3^1; //控制光标移动。
sbit S4 = P3^2; //增大数字。
sbit S5 = P3^3; //降低数字。

uint i;
//将数字的显示（调用stringSend函数放在定时器中断函数内）
uint num = 0;
uint value = 0;
uint y = 12; //数字显示的y轴
uint x = 2;
void writeData(uchar dat);
void writeCmd(uchar cmd);
void setPointer(uint ,uint );
void stringSend(uchar * str);
//flag表示运算符。
uint numar[4] = {0};
uint n1,n2;
uint cnt = 0;

void timerInit()
{
	EA= 1;
	ET0 = 1;//定时器0允许中断
	TR0 = 1; //定时器启动
	TMOD = 0x01; //16位模式，使用定时器0
	TH0 = 0xb7;
	TL0 = 0xff;   //定时20ms，每20ms检查一下数字并显示。
	ET1 = 1;
	TR1 = 1;
	TMOD |= 0x10;
	TH1 = 0xed;
	TL1 = 0xff; //定时器1用于控制答题时间。
}	

void numDisplay() interrupt 1{
	TH0 = 0xb7;
	TL0 = 0xff;
	setPointer(x,y);//在光标指定位置显示一个数字。
	writeData(' ');
	setPointer(x,y);
	writeData('0' + value);
}

void timer1() interrupt 3{
	TH1 = 0xed;
	TL1 = 0xff;
	cnt++;
}
void expGenerate(uchar flag){
	static int seed0 = 0;
	uchar exp[20];
	srand((uint)seed0);
	seed0++;
	n1 = rand() % 100;  //0-99
	n2 = rand() % 100;
	sprintf(exp,"%d %c %d = ?",n1,flag,n2);
	stringSend(exp);
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

void delay(uint n){
	uint i,j;
	for(i = 0;i<n;i++)
		for(j = 0;j<114;j++);
}

//字符串的发送
void stringSend(uchar * str){
	while(*str!='\0'){
		writeData(*str++);
	}
}	

//数据指针移动函数
void setPointer(uint x,uint y){
	if(1 == x){
		writeCmd(0x80 | y);
	}
	else if(2 == x){
		writeCmd(0x80 | 0x40 | y);
	}
}

//LCD初始化
void lcdInit(){
	writeCmd(0x38);
	writeCmd(0x0f);
	writeCmd(0x01); //清屏
	writeCmd(0x06);
}

void keyScan(){
	if(!S2){
		//S2按下，为低电平
		delay(10); //防抖
		if(!S2){
			if(y>12)
				y--;
			setPointer(2,y);
			while(!S2);
		}
	}
	if(!S3){
		//S3按下，为低电平
		delay(10); //防抖
		if(!S3){
			if(y<15)
				y++;
			setPointer(2,y);
			while(!S3);
		}
	}
	if(!S4){
		//S2按下，为低电平
		delay(10); //防抖
		if(!S4){
			if(value < 9)
			{	
				value++;
				numar[y - 12] = value;
			}
			while(!S4);
		}
	}
	if(!S5){
		//S2按下，为低电平
		delay(10); //防抖
		if(!S5){
			if(value > 0){
				value--;
				numar[y - 12] = value;
			}
			while(!S5);
		}
	}
}

void clear(){
	uint i,j;
	for(i = 0;i<2;i++){
		for(j = 0;j<16;j++)
		{
			setPointer(i+1,j);
			writeData(' ');
		}
	}
	

}
int main(){
	uint answer = 0;
	lcdInit();
	timerInit();
	setPointer(1,0);
	expGenerate('*');
	answer = n1 * n2;
	while(1)
	{
		if(cnt == 3000) //每道题目显示15秒钟。
		{
			//检查上一题答案。
			for(i = 0;i<4;i++)
			{
				num = num * 10 + numar[i];
			}
			//结果反馈。
			if(num == answer)
				{
				setPointer(2,0);
				stringSend("RIGHT!");
				}
			else
				{
				setPointer(2,0);
				stringSend("WRONG!");
				}
			num = 0;
			delay(3000);	
			clear();
			setPointer(1,0);
			//答应下一题.
			expGenerate('*');
			answer = n1 * n2;
			value = 0;
			cnt = 0;
		}
		keyScan();
	}
	return 0;
}	