/** PWM ��Ƶ���� ��������������LED�Ƶ�����**/
#include <reg52.h>
#include <intrins.h>

typedef unsigned int uint;
typedef unsigned char uchar;

sfr P4 = 0xe8;
//λ����
sbit S2 = P3^0;
sbit S3 = P3^1;

sbit DOUT = P2^5;
sbit CS = P3^7; //Ƭѡ�źš�
sbit DIN = P2^0;
sbit DCLK = P2^1;
sbit DA_DATA = P4^4;

uchar DA_VAL = 80;  //0-255ȡֵ������PWM��ռ�ձȡ�80������LED��������Сֵ
//DA_VAL / 255 * 5V = voltage
uchar pwmCnt = 0; //��255֮�������0�����ڶ�ʱ��0�жϺ���

void delay(uint n){
	uint i,j;
	for(i=0;i<n;i++)
		for(j=0;j<114;j++);
}

void timer0Init(){
	EA = 1;
	ET0 = 1;
	TMOD = 0x01;
	TL0 = 0xdc;  //100hz
	TH0 = 0xff;
	TR0 = 1;
}

void pwmGenerate() interrupt 1{
	pwmCnt++;
	if(pwmCnt<=DA_VAL){
		DA_DATA = 1; //һ��������Ϊ�ߵ�ƽ�������DA_VAL���ƿ��
	}	
	else{
		DA_DATA = 0;
	}
	TL0 = 0xdc;
	TH0 = 0xff; //ִ��36��֮��ͻ������ 100hz����
}	

void main(){
	timer0Init();
	while(1){ //��s2����led���ȣ���s3�������ȡ�
		if(0 == S2){
			delay(10);
			//����
			if((0 == S2) && DA_VAL > 80){
				DA_VAL--;
				pwmCnt = 0;
				TL0 = 0xdc;
				TH0 = 0xff;
			}
			//while(!S2); ȡ�����ּ�⣬���Ż�һֱ�������ȡ�
		}
		if(0 == S3){
			delay(10);
			//����
			if((0 == S3) && DA_VAL < 255){
				DA_VAL++;	
				pwmCnt = 0;
				TL0 = 0xdc;
				TH0 = 0xff;
			}
			//while(!S3);
		}
	}
}	