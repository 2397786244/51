/**
ʵ����������϶�̬��ʾ1-5.
**/
#include <reg52.h>


sbit DU = P2^6;  //p2.6
sbit WE = P2^7;  //����λѡ 
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
	//������5�ֶ�ѡ������P0����ֵ
	unsigned int num_ch[5]={0x06,0x5b,0x4f,0x66,0x6d};
	//num_ch[0] ��ӡ1��[1]������2��[2]������3��[3]������4 [4]������5
	int i;
	while(1){
	for(i=0;i<5;i++){
		display(num_ch[i],~(0x01<<i));  //����1�ڵ�һ��������ϡ���λѡP0 = ~(0x01)��
	}
	}
}

void display(unsigned int n,unsigned int bits){
		/*��p0=0xff������롣�ȴ�λѡ������һ������ܡ��ر�λѡ���������м��м�΢�����ʱ��
		�򿪶�ѡ����������1����ʽ����ʱ��
		*/
		P0 = 0xFF;
		WE = 1;
		P0 = bits;
		WE = 0;
		
		//��ʱ
		P0 = 0xFF;

		DU = 1;
		P0 = n; 
		DU = 0;
		delay(2);
}	