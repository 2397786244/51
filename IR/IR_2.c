#include <reg52.h>

typedef unsigned int uint;
typedef unsigned char uchar;

bit IR_DECODE_OK; //����������
bit IR_OK; //���������ɡ�
uchar IR_DATA[33];  //����һλ�����룬16λ�û����16λ��ֵ��
uchar IR_BYTE[4]; //����������ݷ���������ĸ��ֽڡ�
uint IR_TIME;

//��ʱ����ʼ�����ⲿ�жϣ���������
void Init(){
	EA = 1;
	TMOD = 0x22; //��λ�Զ���װģʽ��
	TL0 = 0;
	TH0 = 0; //��ʱ��0���ʱ��Ϊ277.76us��
	ET0 = 1; //��ʱ���жϡ�
	TR0 = 1;//��ʱ��������
	
	//�ⲿ�ж�0����
	IT0 = 1; //ѡ�������ش�����ʽ�����ŴӸߵ�ƽ���͵�ƽ��Ч��
	EX0 = 1; //�ⲿ�ж�0����λ��
	
	TL1 = 0xfd;
	TH1 = 0xfd; //����9600�����ʡ�
	SM0 = 0;
	SM1 = 1;
	TR1 = 1;
}

void time0() interrupt 1{
	IR_TIME++; //ÿ���ж���Ҫ256���������ڡ�
}

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
	IR_DECODE_OK = 1;
	return 0;
}	
int main(){
	uint i;
	Init();
	while(1){
		if(IR_OK)
		{
			getIRDecode();
			IR_OK = 0;
			if(IR_DECODE_OK)
			{ //������ɣ����ڷ��ͽ����
				for(i = 0;i<4;i++){
					SBUF = IR_BYTE[i];
					while(!TI);
					TI = 0;
				}
				IR_DECODE_OK = 0;
			}
		}
	}
	return 0;
}	