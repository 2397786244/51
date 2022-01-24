/**
*@brief
* �ⲿ�жϳ���.ʹ���ⲿ�ж�1������֤
***/
#include<reg52.h>

typedef unsigned int uint;
typedef unsigned char uchar;

sbit key = P3^0;
sbit flag = P3^3;
sbit beep = P2^3;

void delay(uint n){
	uint i,j;
	for(i=0;i<n;i++){
		for(j=0;j<114;j++);
	}
}

void int1() interrupt 2{
	//interrupt 2 ��Ӧ�ⲿ�ж�1
	beep = 0;
	while(!key);
	beep = 1;
}

void int1init(){
	EA = 1;
	EX1 = 1;
	IT1 = 0; //0���ǵ͵�ƽ����
}

void main(){
	int1init();
	while(1)
	{	
		flag = 1;
		if(key==0){
			delay(20);
			if(key==0){
				flag = 0;
			}		
		}
	}
}