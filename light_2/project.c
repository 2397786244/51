/**
实现在数码管上动态显示1-5.
**/
#include <reg52.h>


sbit DU = P2^6;  //p2.6
sbit WE = P2^7;  //控制位选 
typedef unsigned char uchar;
void display(unsigned int n,unsigned int);
void delay(unsigned int n){
	int i;
	int j;
	for(i=0;i<n;i++){
		for(j=0;j<144;j++);
	}
}
void main(){
	//数组存放5种段选锁存器P0赋的值
	unsigned int num_ch[5]={0x06,0x5b,0x4f,0x66,0x6d};
	//num_ch[0] 打印1，[1]是数字2，[2]是数字3，[3]是数字4 [4]是数字5
	int i;
	while(1){
	for(i=0;i<5;i++){
		display(num_ch[i],~(0x01<<i));  //数字1在第一个数码管上。即位选P0 = ~(0x01)；
	}
	}
}

void display(unsigned int n,unsigned int bits){
		/*用p0=0xff清除断码。先打开位选，亮第一个数码管。关闭位选锁存器。中间有几微秒的延时。
		打开段选，点亮数字1的样式。延时。
		*/
		P0 = 0xFF;
		WE = 1;
		P0 = bits;
		WE = 0;
		
		//延时
		P0 = 0xFF;

		DU = 1;
		P0 = n; 
		DU = 0;
		delay(2);
}	