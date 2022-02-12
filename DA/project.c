/** ��P4.4�ڷ��������������Ŵ������AIN3�ڡ������������ʾ�����ѹֵ
**/
#include <reg52.h>
#include <intrins.h> //Ϊ_nop_�����ṩԭ��
#define AIN3 0xe4

typedef unsigned int uint;
typedef unsigned char uchar;

sfr P4 = 0xe8; //P4�Ĵ���û����reg52�ж���
//λ����
sbit PWM_DATA = P4^4; //����������зŴ�.��DOUT�������
sbit CS = P3^7;
sbit DCLK = P2^1;
sbit DIN = P2^0;
sbit DOUT = P2^5;

sbit WE = P2^7;
sbit DU = P2^6;

uchar code ch[10] = {0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f};
uchar code pos[4] = {0xfe, 0xfd, 0xfb,0xf7};
uint wei = 0;
uchar pwmCnt = 0;
uchar DA_VAL = 0;  //�趨pwmռ�ձȡ�0-255��ȡֵ��

void delay(uint n){
	uint i,j;
	for(i=0;i<n;i++)
		for(j=0;j<114;j++);
}

void timer0Init(){
	EA = 1; //���жϿ���
	ET0 = 1; //��ʱ��0�ж�
	TMOD = 0x02; //��λ�Զ���װ
	TL0 = 220;
	TH0 = 220; // 100hz,ÿ������������ж���pwm����һ���ߵ�ƽ������͵�ƽ�ܹ�
	// (256 - 220) * 256 * 1.085us Ϊһ��pwm���ڡ�
	TR0 = 1; //��ʱ���򿪡�
}

void pwmGenerate() interrupt 1{
	if(pwmCnt<=DA_VAL){
		PWM_DATA = 1;
	}	
	else{
		PWM_DATA = 0;
	}
	pwmCnt++; //����Զ�Ϊ0
}	

//SPIͨ�ŷ��Ϳ���ָ�
void spiSend(uchar cmd){
	uint i;
	for(i=0;i<8;i++){
		DCLK = 0;
		if(cmd & 0x80){
			DIN = 1;
		}
		else{
			DIN = 0;
		}
		DCLK = 1;
		cmd <<= 1;
	}	
}
//��SPI�����Ͻ������ݡ�
uint spiGet(){
	uint i;
	uint dat = 0;
	for(i=0;i<12;i++){
		dat <<= 1;
		DCLK = 1;
		DCLK = 0; //�½���֮���ȡDOUT��һλ����
		if(DOUT)
			dat |= 0x01;
	}	
	return dat;
}

void display(uint i)
{
	uchar qian, bai, shi, ge;
	static uchar wei;
	qian = i / 1000;
	bai = i % 1000 / 100;
	shi = i % 100 / 10;
	ge = i % 10;
	
	P0 = 0XFF;
	WE = 1;
	P0 = pos[wei];
	WE = 0;
	P0 = 0XFF;
	switch(wei)
	{
		case 0: DU = 1; P0 = ch[qian] | 0x80;	DU = 0; break;
		case 1: DU = 1; P0 = ch[bai]; 	DU = 0; break;	
		case 2: DU = 1; P0 = ch[shi]; 	DU = 0; break;
		case 3: DU = 1; P0 = ch[ge]; 	DU = 0; break;		
	}
	wei++;
	if(wei == 4)
		wei = 0;
}
uint adTrans(uchar cmd){
	uint dat;
	CS = 0; //Ƭѡ�ź���0
	spiSend(cmd);
	//���������ָ�����DCLKΪ0
	DCLK = 0;
	_nop_();
	_nop_();
	_nop_();
	_nop_();
	_nop_();
	dat = spiGet();
	CS = 1;
	return dat;
}	
int main()
{
	uint msCnt;
	uint value;
	DA_VAL = 255; //5v��ѹ
	timer0Init();
	while(1){
		if(1000 <= msCnt){
			msCnt = 0;
			EA = 0; //�ر��жϣ���ֹ��ʱ���жϲ�����PWM���ź����ȡDOUT���ݡ�
			value = adTrans(AIN3);
			value = value * 1.22; // 5000mV/4096
			EA = 1;
		}	
	
	msCnt++;
	display(value);
	delay(1);
	}
	return 0;
}	