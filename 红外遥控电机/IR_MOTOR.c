#include <reg52.h>
/* ң�ذ� 45 �򿪵����47�رյ�� �� 40 ���٣� 44 ���� */
typedef unsigned char uchar;
typedef unsigned int uint;

sbit AIN = P1^0;
//sbit BIN = P1^1;

bit IR_DECODE_OK; //��־����������
bit IR_OK; //��־�������
uchar IR_DATA[33];  //����һλ�����룬16λ�û����16λ��ֵ��
uchar IR_BYTE[4]; //����������ݷ���������ĸ��ֽڡ���ȡ�������ֽ��ж� ����
uint IR_TIME;
uchar pwm;
uchar pwm_length = 120; //���170
enum STATES {OPEN,OFF};
enum STATES NOW_STATES = OFF; //Ĭ�Ϲرա�
int getIRDecode();

//��ʱ����ʼ�����ⲿ�жϣ���������
void Init(){
	EA = 1;
	TMOD = 0x02; //��λ�Զ���װģʽ��
	TL0 = 0;
	TH0 = 0; //��ʱ��0���ʱ��Ϊ277.76us��
	ET0 = 1; //��ʱ���жϡ�
	TR0 = 1;//��ʱ��������
	//�ⲿ�ж�0����
	IT0 = 1; //ѡ�������ش�����ʽ�����ŴӸߵ�ƽ���͵�ƽ��Ч��
	EX0 = 1; //�ⲿ�ж�0����λ��
	TMOD |= 0x20;
	TH1 = TL1 = 220;
	TR1 = 0;
	ET1 = 0;
	//�����ʼ����
	AIN = 1;
	//BIN = 1;
}

void time0() interrupt 1{
	IR_TIME++; //ÿ���ж���Ҫ256���������ڡ�
}

void time1() interrupt 3{
	pwm++;
	if(pwm == 255){
		AIN = 1;
	}
	if(pwm_length == pwm){
		AIN = 0;
	}
}
//���յ������źţ������ⲿ�жϡ�
void getIRData() interrupt 0{
	static uint i;
	static bit flag;
	if(flag)
		{
		//�ж�һ��ʼ���յ����Ƿ�Ϊ��ʼ�롣����������׿�ʼ�档
		if((IR_TIME >= 32)&& (IR_TIME < 53)) //8.8ms-14.5ms
		{
			i = 0;
		}
		IR_DATA[i] = IR_TIME; //������������档
		i++;
		IR_TIME = 0;
		if(33 == i)
		{
			IR_OK = 1;
			i = 0; //�Ѿ�������33λ���ݡ�
			flag = 0; //flag��0׼����һ�ν����źš�
		}
	}
	else{
		flag = 1;
		IR_TIME = 0;
	}
}	
int getIRDecode(){
	uint i ,j , k = 1; //k�������±ꡣ���������롣
	for(i = 0;i<4;i++){
		for(j = 0;j<8;j++){
			if(IR_DATA[k] > 5){
				//��λ����Ϊ1.
				IR_BYTE[i] |= 0x80;
			}
			if(j < 7)			
				IR_BYTE[i] >>= 1;
			k++;
		}
	}
	return 0;
}	

void IR_Press_Judge(){
	switch(IR_BYTE[2]){
		case 0x45: if(NOW_STATES != OPEN) NOW_STATES = OPEN; TR1 = 1;ET1 = 1;break;
		case 0x47: if(NOW_STATES != OFF) NOW_STATES = OFF; TR1 = 0;ET1 = 0;AIN = 0;break;
		case 0x40: if(pwm_length < 160) pwm_length+=10;break;
		case 0x44: if(pwm_length > 10) pwm_length-=10;break;
	}
}
int main(){
	Init();
	while(1){
		if(IR_OK){
			IR_OK = 0;
			getIRDecode();
			IR_Press_Judge();
		}
	}
}
