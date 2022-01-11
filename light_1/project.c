/**
*数码管静态显示\动态显示
**/
#include <reg52.h>
#include <intrins.h>

typedef unsigned int uint;
typedef unsigned char uchar;

sbit DU = P2^6; //数码管段选 LE口。1的时候输出端随输入端变化。位选锁存器同理
sbit WE = P2^7; //数码管位选

void delay(uint n)
	{
	int i,j;
	for(i=0;i<n;i++)
		{
		for(j=0;j<114;j++);
		}
	}
void main(){
	
	/*WE= 1; //打开位选锁存器
	P0 = 0xFE;  //1111 1110 选择第一个数码管打开
	WE = 0; //关闭位选锁存器 
	
	DU = 1;
	P0 = 0x06;
	DU = 0;*/
	//控制段选。段选要求高电平，点亮该段数码管。 
	//段选锁存器关闭
	while(1){
		
		P0 = 0xFF; //清除断码
		WE = 1;
		P0 = 0xFE; 	//第一个管子
		WE = 0;
		
		DU = 1;
		P0 = 0x06;
		DU = 0; //显示1
		
		delay(10);
		
		P0 = 0xFF;	//清除断码	
		WE = 1;
		P0 = 0XFD; //选择第二个管子
		WE = 0;
		
		DU = 1;
		P0 = 0x5B; //点亮一个2的数字
		DU = 0;
		delay(3);
		
		//在第三个管子上显示3
		P0 = 0XFF;
		WE = 1;
		P0 = 0XFB;
		WE = 0;	//1111 1011
		
		DU = 1;
		P0 = 0x4F; //显示3，打开 abcdg 0100 1111
		DU = 0;
		delay(3);
	}

}