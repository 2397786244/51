/**
* ����ͨ�š���Ƭ���������֣��������ʾ���������̰���S2�ۼ�value����������
*/

#include <reg52.h>

typedef unsigned int uint;
typedef unsigned char uchar;

uchar code table[10]={0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f};
uint code pos[3] = {0xfe,0xfd,0xfb};
uint value=0;
uint num=0;

sbit WE = P2^7;
sbit DU = P2^6;
sbit key_s2 = P3^0;

void delay(uint n);
void duli_key();
void display(uint num);

//��ʱ��0�Ͷ�ʱ��1�ʹ���ͨ�ų�ʼ��
void timerInit(){
	EA = 1;//�ܿ���λ
	ET0 = 1;
	ET1 = 1;//��ʱ��0��1���жϴ�
	ES = 1; //�����жϴ�
	TR0 = 1;
	TR1 = 1;//����0��1������ʱ��
	//����ʱ��0��1����ֵ
	//��ʱ��1���ڴ���ͨ�š�ģʽΪ8λ�Զ���װ
	TMOD = 0x21; //��ʱ��1������Ϊ0x20;
	TL0 = 0xff;
	TH0 = 0xed;
	TL1 = 0xfd;
	TH1 = 0xfd;//9600������
	
	//�����жϵ�����
	REN = 1;//���տ���λ
	SM0 = 0;
	SM1 = 1;
}
//��ʱ������������̵�ɨ��
void delay(uint n){
	uint i,j;
	for(i=0;i<n;i++)
		for(j=0;j<114;j++);
}

//�������ʾ
void display(uint num){
	static uint wei=0;
	uint bai;
	uint shi;
	uint ge;
	bai = num /100;
	shi = num/10 % 10;
	ge = num % 10;
	
	//ȷ��λѡ
	P0 = 0xff;
	WE = 1;
	P0 = pos[wei];
	WE = 0;
	switch(wei){
		case 0:DU = 1;P0 = table[bai];DU = 0;break;
		case 1:DU = 1;P0 = table[shi];DU = 0;break;
		case 2:DU = 1;P0 = table[ge];DU = 0;break;
	}
	wei++;
	if(wei==3)
		wei = 0;
}

//��ʱ��0���жϺ���.����display����ʱ��ʾ
void timer0() interrupt 1{
	//��TL0��TH0���¸�ֵΪ0xff��0xed;
	TH0 = 0xed;
	TL0 = 0xff;
	display(num);
}

//�����жϺ���
void uart() interrupt 4{
	//���ж�TI�Ƿ�Ϊ1.�Ƿ������ж�
	if(TI){
		TI = 0;
	}
	if(RI){
		//�����жϱ�־λ
		num = SBUF;
		RI = 0;
	}
}
//��������
void duli_key(){
	//key_s2һ��ʼ��1
	if(!key_s2){
		delay(10);
		if(!key_s2){
			value++;
			SBUF = value;
		}
		while(!key_s2);
	}
}

void main(){
	timerInit();
	while(1){
		duli_key();
	}
}
