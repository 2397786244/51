#include <reg52.h>

typedef unsigned char uchar;
typedef unsigned int uint;

sbit RS = P3^5; //数据，命令端
sbit RW = P3^6; //读，写
sbit EN = P3^4; //使能端

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
int main(){
	uint i = 0,j = 0;
	//设置显示指令。
	writeCmd(0x38);	
	writeCmd(0x0c);
	//开显示指令
	writeCmd(0x01); //清屏指令
	//数据指针设置
	while(1){
		//写一个字符后指针往后+1.
		writeCmd(0x06);
		writeCmd(0x80 | i); //从第一行最左侧开始移动。
		writeData('b');
		writeData('i');
		writeData('g');
		writeData('D');
		writeData('r');
		writeData('e');
		writeData('a');
		writeData('m');
		writeData('e');
		writeData('r');
		delay(300);
		writeCmd(0x80 | 0x40 | (15 - j)); //从第二行最右侧往左移动。
		writeData('W');
		writeData('C');
		writeData('Z');
		delay(300);
		writeCmd(0x01); //清屏指令。
		i++;
		j++;
		if(i == 16)
			i = 0; //向右移动显示bigDreamer.
		if(j == 16)
			j = 0; //显示WCZ
	}
	return 0;
}
