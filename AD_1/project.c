#include<reg52.h>
#include<intrins.h>

#define CMD_AIN1 0x94 
#define CMD_AIN2 0xd4
#define CMD_AIN4 0xa4
#define CMD_AIN3 0xe4
//����AIN1 ����AIN2 ��λ��AIN4 �ⲿ����AIN3
typedef unsigned int uint;
typedef unsigned char uchar;

sbit WE = P2^7;
sbit DU = P2^6;
sbit DIN = P2^0; //�������������
sbit DCLK = P2^1;  //�ṩʱ����
sbit CS = P3^7;
sbit DOUT = P2^5; //�������
uchar code ch[10] = {0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f};
uchar code pos[4] = {0xfe, 0xfd, 0xfb,0xf7};
uint wei = 0;

//��ʱ����
void Delay_Ms(uint ms)
{
     uint i;
	 do{
	      i = 11059200UL / 96000; 
		  while(--i);   //96T per loop
     }while(--ms);
}

void display(uint i)
{
	uchar qian, bai, shi, ge;
	static uchar wei;
	qian = i / 1000;
	bai = i % 1000 / 100;
	shi = i % 100 / 10;
	ge = i % 10;
	
	P0 = 0XFF;//�������
	WE = 1;//��λѡ������
	P0 = pos[wei];
	WE = 0;//����λѡ����
	P0 = 0XFF;//�������
	switch(wei)
	{
		case 0: DU = 1; P0 = ch[qian] | 0x80;	DU = 0; break;//0x80����С����
		case 1: DU = 1; P0 = ch[bai]; 	DU = 0; break;	
		case 2: DU = 1; P0 = ch[shi]; 	DU = 0; break;
		case 3: DU = 1; P0 = ch[ge]; 	DU = 0; break;		
	}
	wei++;
	if(wei == 4)
		wei = 0;
}

void spiSend(uchar cmd){
	uint i;
	for(i=0;i<8;i++){
		DCLK = 0;
		if(cmd & 0x80){
			DIN = 1;
		}	
		else
			DIN = 0;
		DCLK = 1;//�����ص�ʱ������һλ����
		cmd <<= 1;
	}	
}

uint spiRead(){
	uint i;
	uint dat = 0;
	for(i = 0;i<12 ;i++){
		//ѡ��XPT2604 12λģʽ
		dat <<= 1;
		DCLK = 1;
		DCLK = 0; //�½��ص�ʱ���DOUT�϶�һλ���ݡ�
		if(DOUT)
			dat |= 0x01;
	}
	return dat;
}	

uint adTrans(uchar cmd){
	uint dat;
	CS = 0; //Ƭѡ�ź�����0
	spiSend(cmd); //���Ϳ���ָ��
	DCLK = 0; //����ָ����Ȱ�DCLK����
	_nop_();
	_nop_();
	_nop_();
	_nop_();
	_nop_();
	//��ʱ25us���ҡ�
	//��ʱ�󣬿��Դ�DOUT�϶�����
	dat = spiRead();
	CS = 1; //ת����������CS����
	return dat;
}

void main(){
	uint msCnt;
	uint value;
	while(1){
		if(100 <= msCnt)
		{
			msCnt = 0;
			value = adTrans(CMD_AIN1); //ÿ500ms����һ��ת����
			value *= 1.2207; //5000mv/4096��ת�� valueʵ���ϵõ�����0-4095֮�������
		}
		msCnt++;
		display(value);
		Delay_Ms(5);
	}	
}	