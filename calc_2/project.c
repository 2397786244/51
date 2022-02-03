/**
*@brief
* ������ ,�������ʾ�ڵ������ϡ�
***/
#include<reg52.h>
#include<intrins.h>
#include<stdlib.h>

typedef unsigned int uint;
typedef unsigned char uchar;

uchar code table[10]={0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f};
uint code num_key[16] = {1,2,3,0,4,5,6,0,7,8,9,0,0,0,0,0};
uint code pos[4] = {0xfe, 0xfd, 0xfb,0xf7};
uint wei=0;
//�������λ��ʾ���֡�
//ʹ�þ�����̣�����ܣ���������

//���ȶ��������
sbit DU = P2^6;
sbit WE = P2^7;
//�������ʹ��P3IO��
//sbit DIO = P3^4;	//14��
//sbit S_CLK = P3^5; //11��
//sbit R_CLK = P3^6;	//12��
//������
uint answer=0; //�������µİ����γɵ���ֵ��
uint key_press_cnt=0; //���´�����ͳ��
uint calc_type = 0; //3 + 7 - 11 * 15 /
uint number1=0; //����������ż���answer�浽number1�
//���µ��ڣ���ʾ����� 14 = 
uint value = 0; //������Ӧ����ֵ��
uint res=0; //��ʾ����ֵ��
//����ԭ��
void display(uint n);
void delay(uint n);
void keyscan();
void timerInit();

void delay(uint n){
	uint i,j;
	for(i=0;i<n;i++)
		for(j=0;j<114;j++);
}

//�����������1����õ�������ֵ��
void keyscan(){
	//�Ƚ����м�⡣
	P3 = 0xF0;
	if(0xF0!=P3){
		delay(11);//�������
		if(0xF0!=P3)
		{
			switch(P3)
			{
				case 0xE0: value = 0;break;
				case 0xD0: value = 1;break;
				case 0xB0: value = 2;break;
				case 0x70: value = 3;break;
			}	
		}
		//�����м��
		P3 = 0x0F;
		switch(P3){
			case 0x0E:value += 0;break;
			case 0x0D:value += 4;break;
			case 0x0B:value += 8;break;
			case 0x07:value += 12;break;
		}
		key_press_cnt++;
		
		if(14==value){
			key_press_cnt--;
		}
		if(key_press_cnt>2)
		{	
			answer=0;
			key_press_cnt = 1;
		}
		//�������2�Ρ��ٰ��µ�ʱ����answer = 0
		if((value!=3)&&(value!=7)&&(value!=11)&&(value!=15)&&(value!=14))
		{
			answer = answer * 10 + num_key[value];
			res = answer;
		}
		while(0x0F!=P3); //���ּ��
		}
	//��value��ֵת��Ϊ���֡�
	//�����Ļ��ڵ��S21֮ǰ�������а��µ�ֵ����answer�
}
void timerInit(){
	EA= 1;
	ET0 = 1;//��ʱ��0�����ж�
	TR0 = 1; //��ʱ������
	TMOD = 0x01; //16λģʽ��ʹ�ö�ʱ��0
	TH0 = 0xed;
	TL0 = 0xff;   //��ʱ5ms���ӵ�60927�ο�ʼ��
	ET1 = 1;
	TR1 = 1;
	TMOD |= 0x10;
	TH1 = 0xdb;
	TL1 = 0xff; //��ʱ��1���ڼ��������ŵİ��£�= �ŵİ��¡�
	//��ʱ10ms
}
void timer0() interrupt 1{
	//��ʱ��0
	TH0 = 0xed;
	TL0 = 0xff;
	display(res);
}
void timer1() interrupt 3 {
	TH0 = 0xdb;
	TL0 = 0xff;
	switch(value)
	{
		case 3:number1 = res;answer = 0;key_press_cnt = 0;calc_type = 1;value = 0;break; // +
		case 7:number1 = res;answer = 0;key_press_cnt = 0;calc_type = 2;value = 0;break; // -
		case 11:number1 = res;answer = 0;key_press_cnt = 0;calc_type = 3; value = 0;break;//*
		case 15:number1 = res;answer = 0;key_press_cnt = 0;calc_type = 4; value = 0;break;// /
	}
	if(14==value)
	{
		switch(calc_type){
			case 1: res = number1 + answer; calc_type = 0;break;
			case 2: res = number1 - answer; calc_type = 0;break;
			case 3: res = number1 * answer; calc_type = 0;break;
			case 4: res = number1 / answer; calc_type = 0;break;
			default:break;
		}
		key_press_cnt = 0;
		answer = 0;
		value = 0;
	}	
}

void display(uint n){
	uint qian;
	uint bai;
	uint shi;
	uint ge;
	uint num_len;
	qian = n/1000;
	bai = n /100 % 10;
	shi = n /10 % 10;
	ge = n%10;
	if(qian){
		num_len = 4;
	}
	else if(bai){
		num_len = 3;
	}
	else if(shi){
		num_len = 2;
	}
	else{
		num_len = 1;
	}
	P0 = 0xff;
	WE = 1;
	P0 = pos[wei];
	WE = 0;
	switch(wei){
		case 0 : DU = 1;P0 = (num_len<4) ? 0x00 : table[qian];DU = 0;break;
		case 1 : DU = 1;P0 = (num_len<=2) ? 0x00 : table[bai];DU = 0;break;
		case 2 : DU = 1;P0 = (1==num_len) ? 0x00 : table[shi];DU = 0;break;
		case 3 : DU = 1;P0 = table[ge];DU = 0;break;
	}
	wei++;
	if(4==wei){
		wei = 0;
	}
}

void main(){
	timerInit();
	while(1){
		keyscan();
	}
}

