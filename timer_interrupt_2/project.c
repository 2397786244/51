/**
*@brief
* ��ʱ����ʱ
* ��P1.0IO������P3.4�������źţ�ʹ����������
* �����������ʾ��ֵ��
* ��P3.5�ڽ�P1.0
***/
#include<reg52.h>

typedef unsigned int uint;
typedef unsigned char uchar;
uint mSec=0;
uint sec=0;
uchar ch[10] = {0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f};

sbit DU = P2^6;
sbit WE = P2^7;
sbit LED1 = P1^0;

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
	//timer0ʵ�ֶ�ʱ���Ĺ��ܡ���Ӳ����ʱ��
	TR0 = 1;
	TMOD |= 0x01; 
	TH0 = 0x4b;
	TL0 = 0xd;
}
void timer1init(){
	TR1 = 1; //��ʱ������1��Ϊ����������
	TMOD |= 0x50;
	TH1 = 0;
	TL1 = 0; //�Ͱ�λ
}

void main(){
	timer0init();
	timer1init();//��������ʱ��������ʼ����
	while(1)
	{
		if(TF0)
		{
			//timer0�Ѿ����
			mSec++; //��ȥ��50ms
			TH0 = 0x4b;
			TL0 = 0xfd;
			TF0 = 0;
			if(20==mSec)
			{
				//��ȥ��1000ms
				sec++;
				mSec = 0;
				//��LED1����һ������
				LED1 = ~LED1;
				LED1 = ~LED1; //���������ᵼ�¿�����LED������
				//���԰�if������Ϊ(10==mSec)

			}
			//���ϴ���ʹ�ü������ܹ�ÿ1000ms����1
		}
		if(TL1>99)TL1 = 0;
		display(TL1);
	}
}
