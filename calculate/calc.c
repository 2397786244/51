/**
*尝试：简易计算器 +-*功能
**/
#include <reg52.h>

typedef unsigned int uint;
typedef unsigned char uchar;

void delay(uint n);
void keyscan();
void display(uint n);
int transform(uint n);
int calcc(uint n1,uint n2,uint type);

//数码管位选、段选
sbit WE = P2^7;
sbit DU = P2^6;

//LED灯
sbit led = P1^0;

//按下按键的值。
uint value; //0-数字1，1-数字2，2-数字3，4-数字4，5-数字5，6-数字6，8-数字7，
//9-数字8，10-数字9，13-数字0，14- =号。+-3 - - 7, * - 11, 整除 15
//按下其他没有作用的按键，led亮2秒。除数为0，灯亮1秒。

uchar calc; //运算符号
uint table[10] = {0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f};
//代表数字0-9 

void delay(uint n){
	int i,j;
	for(i=0;i<n;i++){
		for(j=0;j<114;j++);
	}
}
void keyscan(){
	P3 = 0xf0; //列扫描 
	if(P3 != 0xf0)
	{
		//说明按下了一个按键。
		delay(10);//软件消抖
		if(P3!=0xf0)
		{
			switch(P3)
				{
				case 0xe0 : value = 0;break;
				case 0xd0 : value = 1;break;
				case 0xb0 : value = 2;break;
				case 0x70 : value = 3;break;
				}//按键按下的四种情况	
			P3 = 0x0f;
			//行扫描
			switch(P3){
				case 0x0e : value += 0;break;
				case 0x0d : value += 4;break;
				case 0x0b : value += 8;break;
				case 0x07 : value += 12;break;
			}
			while(P3!=0x0f); //松手检测。在行检测之后
		}
	}
}
int transform(uint n){
	uint num=0;
	switch(n)
	{
		case 0:num=1;break;
		case 1:num=2;break;
		case 2:num=3;break;
		case 4:num=4;break;
		case 5:num=5;break;
		case 6:num=6;break;
		case 8:num=7;break;
		case 9:num=8;break;
		case 10:num=9;break;
		case 13:num=0;break;
	}
	return num;
}

int calcc(uint n1,uint n2,uint type)
{
	uint result;
	switch(type)
	{
		case 3: result = n1+n2;break; //n1+n2;
		case 7:
			result = (n1>n2) ? n1-n2 : n2-n1 ;break;
		case 11: result = n1*n2;break;
		
		//整除暂时不做
	}
	return result;
}

void display(uint n)
{
	uint shi,ge,i=0;
	shi = n/10;
	ge = n%10;
	while(1)
	{
	if(shi!=0)
	{
		P0 = 0xff;
		WE = 1;
		P0 = ~0x01; //第一个数码管
		WE = 0;
		
		DU = 1;
		P0 = table[shi];
		DU = 0; //显示十位。
		delay(10);
	}
	P0 = 0xff;
	WE = 1;
	P0 = ~0x02;
	WE = 0;
	
	DU = 1;
	P0 = table[ge];
	DU = 0; //显示个位
	delay(10);
	if(n==0)break;
	i++;
	if(i==114*1000)
		break;
	}
}
void main()
{
	uint n1=0,n2=0,type;
	while(1)
	{
		display(0);
		//第一个数字
		P3 = 0xf0;
		while(P3==0xf0);
		keyscan();
		n1 = transform(value);
		//这里要等待输入运算符
		P3 = 0xf0;
		while(P3==0xf0);
		keyscan();
		type = value;
		//等待输入第二个数字
		P3 = 0xf0;
		while(P3==0xf0);
		keyscan();
		n2 = transform(value);
		display(calcc(n1,n2,type));
	}
}