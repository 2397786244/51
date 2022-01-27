//�������λ��ʾ������0-99
//��ʱ������һ��50����
#include <reg52.h>

typedef unsigned int uint;
typedef unsigned char uchar;

sbit WE = P2^7;
sbit DU = P2^6;
sbit key_s2 = P3^0; //������������԰Ѷ�ʱ�����㡣
sbit int_0 = P3^2;

uchar ch[10] = {0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f};
uint sec=0,mSec=0;

void delay(uint n){
	uint i,j;
	for(i=0;i<n;i++)
		for(j=0;j<114;j++);
}
void display(uint num){
	uint shi;
	uint ge;
	shi = num / 10;
	ge = num % 10;
	
	P0 = 0xff;
	WE = 1;
	P0 = ~0x01;
	WE = 0;
	
	DU = 1;
	P0 = ch[shi];
	DU = 0;
	delay(10);
	
	P0 = 0xff;
	WE= 1;
	P0 = ~0x02;
	WE = 0;
	
	DU = 1;
	P0 = ch[ge];
	DU = 0;
	delay(10);
}

void timer0init(){
	EA=1;
	ET0 = 1; //��ʱ���жϴ� IE�Ĵ���
	IT0 = 0; //�͵�ƽ�������ⲿ�ж�0
	TMOD = 0x01; //M1 = 0,M0 = 1��16λȫ���Ķ�ʱ��ģʽ
	TR0 = 1;  //��ʱ��0�򿪡� 
	EX0 = 1; //�ⲿ�ж�0
	TH0 = 0x4B;
	TL0 = 0xfd; //��ʱ����ʼֵ���������Ҫ50����
}

void keyint()interrupt 0{
	//����S2��ʱ�򣬶�ʱ�����Ϊ0�����¿�ʼ��ʱ
	sec = 0;
	mSec = 0;
	TH0 = 0x4b;
	TL0 = 0xfd;
	while(!key_s2); //���ּ�⡣
}

void timer0()interrupt 1{
	mSec++;  //˵���Ѿ�������50ms
	//��TH0 TL0�ٴθ�ֵ
	TH0 = 0x4b;
	TL0 = 0xfd;
	if(20==mSec){
		mSec = 0;
		sec++;
	}
	if(sec>99)sec = 0;
}
void main(){
	timer0init(); //��ʱ����ʼ��
	while(1){
		int_0 = 1;
		if(0==key_s2){
			delay(20);
			if(0==key_s2){
				int_0 = 0;
				//���ⲿ�ж�0һ���͵�ƽ��	
			}
		}
		display(sec);
	}
}