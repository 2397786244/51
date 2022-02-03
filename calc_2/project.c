/**
*@brief
* 计算器 ,运算符显示在点阵屏上。
***/
#include<reg52.h>
#include<intrins.h>
#include<stdlib.h>

typedef unsigned int uint;
typedef unsigned char uchar;

uchar code table[10]={0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f};
uint code num_key[16] = {1,2,3,0,4,5,6,0,7,8,9,0,0,0,0,0};
uint code pos[4] = {0xfe, 0xfd, 0xfb,0xf7};
uint wei=0;
//最左边四位显示数字。
//使用矩阵键盘，数码管，点阵屏。

//首先定义数码管
sbit DU = P2^6;
sbit WE = P2^7;
//矩阵键盘使用P3IO口
//sbit DIO = P3^4;	//14脚
//sbit S_CLK = P3^5; //11脚
//sbit R_CLK = P3^6;	//12脚
//点阵屏
uint answer=0; //连续按下的按键形成的数值。
uint key_press_cnt=0; //按下次数的统计
uint calc_type = 0; //3 + 7 - 11 * 15 /
uint number1=0; //按下运算符号键，answer存到number1里。
//按下等于，显示结果。 14 = 
uint value = 0; //按键对应的数值。
uint res=0; //显示的数值。
//函数原型
void display(uint n);
void delay(uint n);
void keyscan();
void timerInit();

void delay(uint n){
	uint i,j;
	for(i=0;i<n;i++)
		for(j=0;j<114;j++);
}

//如果函数返回1，则得到的是求值。
void keyscan(){
	//先进行列检测。
	P3 = 0xF0;
	if(0xF0!=P3){
		delay(11);//软件消抖
		if(0xF0!=P3)
		{
			switch(P3)
			{
				case 0xE0: value = 0;break;
				case 0xD0: value = 1;break;
				case 0xB0: value = 2;break;
				case 0x70: value = 3;break;
			}	
		}
		//进行行检测
		P3 = 0x0F;
		switch(P3){
			case 0x0E:value += 0;break;
			case 0x0D:value += 4;break;
			case 0x0B:value += 8;break;
			case 0x07:value += 12;break;
		}
		key_press_cnt++;
		
		if(14==value){
			key_press_cnt--;
		}
		if(key_press_cnt>2)
		{	
			answer=0;
			key_press_cnt = 1;
		}
		//如果按了2次。再按下的时候让answer = 0
		if((value!=3)&&(value!=7)&&(value!=11)&&(value!=15)&&(value!=14))
		{
			answer = answer * 10 + num_key[value];
			res = answer;
		}
		while(0x0F!=P3); //松手检测
		}
	//把value的值转换为数字。
	//这样的话在点击S21之前，把所有按下的值放在answer里。
}
void timerInit(){
	EA= 1;
	ET0 = 1;//定时器0允许中断
	TR0 = 1; //定时器启动
	TMOD = 0x01; //16位模式，使用定时器0
	TH0 = 0xed;
	TL0 = 0xff;   //定时5ms，从第60927次开始。
	ET1 = 1;
	TR1 = 1;
	TMOD |= 0x10;
	TH1 = 0xdb;
	TL1 = 0xff; //定时器1用于检查运算符号的按下，= 号的按下。
	//定时10ms
}
void timer0() interrupt 1{
	//定时器0
	TH0 = 0xed;
	TL0 = 0xff;
	display(res);
}
void timer1() interrupt 3 {
	TH0 = 0xdb;
	TL0 = 0xff;
	switch(value)
	{
		case 3:number1 = res;answer = 0;key_press_cnt = 0;calc_type = 1;value = 0;break; // +
		case 7:number1 = res;answer = 0;key_press_cnt = 0;calc_type = 2;value = 0;break; // -
		case 11:number1 = res;answer = 0;key_press_cnt = 0;calc_type = 3; value = 0;break;//*
		case 15:number1 = res;answer = 0;key_press_cnt = 0;calc_type = 4; value = 0;break;// /
	}
	if(14==value)
	{
		switch(calc_type){
			case 1: res = number1 + answer; calc_type = 0;break;
			case 2: res = number1 - answer; calc_type = 0;break;
			case 3: res = number1 * answer; calc_type = 0;break;
			case 4: res = number1 / answer; calc_type = 0;break;
			default:break;
		}
		key_press_cnt = 0;
		answer = 0;
		value = 0;
	}	
}

void display(uint n){
	uint qian;
	uint bai;
	uint shi;
	uint ge;
	uint num_len;
	qian = n/1000;
	bai = n /100 % 10;
	shi = n /10 % 10;
	ge = n%10;
	if(qian){
		num_len = 4;
	}
	else if(bai){
		num_len = 3;
	}
	else if(shi){
		num_len = 2;
	}
	else{
		num_len = 1;
	}
	P0 = 0xff;
	WE = 1;
	P0 = pos[wei];
	WE = 0;
	switch(wei){
		case 0 : DU = 1;P0 = (num_len<4) ? 0x00 : table[qian];DU = 0;break;
		case 1 : DU = 1;P0 = (num_len<=2) ? 0x00 : table[bai];DU = 0;break;
		case 2 : DU = 1;P0 = (1==num_len) ? 0x00 : table[shi];DU = 0;break;
		case 3 : DU = 1;P0 = table[ge];DU = 0;break;
	}
	wei++;
	if(4==wei){
		wei = 0;
	}
}

void main(){
	timerInit();
	while(1){
		keyscan();
	}
}

