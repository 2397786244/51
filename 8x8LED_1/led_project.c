/**
*@brief
*8*8点阵屏的显示—1
**/
#include<reg52.h>

//IO口的定义。点阵用了三个IO口，用三个位定义
sbit DIO=P3^4;  //14脚，串行输入。
sbit S_CLK=P3^5;	//11脚。上升沿的时候把DS端的数据放到移位寄存器中。
sbit R_CLK=P3^6;	//12,输出锁存器。上升沿的时候并行输?
void main(){
	//实现点亮一个点
	//行线0x80;
	//列线0xFE;
	//先发列线。让列线的数据进入第二片595的寄存器?
	unsigned int i,dat;
	while(1){
	dat=0xfe; //1111 1110.对应列线上Q7-Q0
	for(i=0;i<8;i++){
		S_CLK=0;
		R_CLK=0;
		//给11脚一个上升沿
		if(dat&0x01){
			DIO = 1; //位检查，如果dat最低位为1，则DIO串行输入一个1
		}
		else{
			DIO = 0;
		}
		S_CLK = 1;
		//把这个数据送到寄存器
		dat >>= 1; //移位。把次低位放到最低位上
	}
	dat=0x80;
	for(i=0;i<8;i++){
		S_CLK=0;
		R_CLK=0;
		if(dat&0x01){
			DIO = 1; //位检查，如果dat最低位为1，则DIO串行输入一个1
		}
		else{
			DIO = 0;
		}
		S_CLK = 1;
		dat >>= 1;
	}
	R_CLK = 1; //给12脚上升沿，进行并行输出
	}
}