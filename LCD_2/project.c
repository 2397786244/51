/*��ʽ��ʾ����������*/
#include <reg52.h>
#include <stdlib.h>
#include <stdio.h>

typedef unsigned char uchar;
typedef unsigned int uint;

sbit RS = P3^5; //���ݣ������
sbit RW = P3^6; //����д
sbit EN = P3^4; //ʹ�ܶ�
sbit S2 = P3^0;
sbit S3 = P3^1; //���ƹ���ƶ���
sbit S4 = P3^2; //�������֡�
sbit S5 = P3^3; //�������֡�

uint i;
//�����ֵ���ʾ������stringSend�������ڶ�ʱ���жϺ����ڣ�
uint num = 0;
uint value = 0;
uint y = 12; //������ʾ��y��
uint x = 2;
void writeData(uchar dat);
void writeCmd(uchar cmd);
void setPointer(uint ,uint );
void stringSend(uchar * str);
//flag��ʾ�������
uint numar[4] = {0};
uint n1,n2;
uint cnt = 0;

void timerInit()
{
	EA= 1;
	ET0 = 1;//��ʱ��0�����ж�
	TR0 = 1; //��ʱ������
	TMOD = 0x01; //16λģʽ��ʹ�ö�ʱ��0
	TH0 = 0xb7;
	TL0 = 0xff;   //��ʱ20ms��ÿ20ms���һ�����ֲ���ʾ��
	ET1 = 1;
	TR1 = 1;
	TMOD |= 0x10;
	TH1 = 0xed;
	TL1 = 0xff; //��ʱ��1���ڿ��ƴ���ʱ�䡣
}	

void numDisplay() interrupt 1{
	TH0 = 0xb7;
	TL0 = 0xff;
	setPointer(x,y);//�ڹ��ָ��λ����ʾһ�����֡�
	writeData(' ');
	setPointer(x,y);
	writeData('0' + value);
}

void timer1() interrupt 3{
	TH1 = 0xed;
	TL1 = 0xff;
	cnt++;
}
void expGenerate(uchar flag){
	static int seed0 = 0;
	uchar exp[20];
	srand((uint)seed0);
	seed0++;
	n1 = rand() % 100;  //0-99
	n2 = rand() % 100;
	sprintf(exp,"%d %c %d = ?",n1,flag,n2);
	stringSend(exp);
}

//�ж�Һ�����Ƿ����
void readStates(){
	uchar flag = 0;
	P0 = 0xff; //��P0�Ĵ�������D0-D7�����ߡ�
	RS = 0;  //ָ��ѡ��
	RW = 1;  //��ѡ��
	do{
		EN = 1;  //ʹ��
		flag = P0;  //�����ݿڸ�ֵ��flag���ж�flag�ڰ�λ(D7)�Ƿ�Ϊ0��Ϊ0������˳�ѭ����
		EN = 0;
	}while(flag & 0x80);
}

void writeCmd(uchar cmd){
	//ÿ�η���ָ��ǰ�ж������Ƿ����
	readStates();
	RS = 0;
	RW = 0;
	P0 = cmd;
	EN = 1;
	EN = 0;//ָ�����֮�����͡�����ʹ���źŸı����֮������ʱ��涨�����ֵ֮��
}	

void writeData(uchar dat){
	readStates();
	RS = 1;  //ѡ������
	RW = 0;  //д
	P0 = dat; 
	EN = 1;
	EN = 0;//ʹ�ܱ���150ns
}

void delay(uint n){
	uint i,j;
	for(i = 0;i<n;i++)
		for(j = 0;j<114;j++);
}

//�ַ����ķ���
void stringSend(uchar * str){
	while(*str!='\0'){
		writeData(*str++);
	}
}	

//����ָ���ƶ�����
void setPointer(uint x,uint y){
	if(1 == x){
		writeCmd(0x80 | y);
	}
	else if(2 == x){
		writeCmd(0x80 | 0x40 | y);
	}
}

//LCD��ʼ��
void lcdInit(){
	writeCmd(0x38);
	writeCmd(0x0f);
	writeCmd(0x01); //����
	writeCmd(0x06);
}

void keyScan(){
	if(!S2){
		//S2���£�Ϊ�͵�ƽ
		delay(10); //����
		if(!S2){
			if(y>12)
				y--;
			setPointer(2,y);
			while(!S2);
		}
	}
	if(!S3){
		//S3���£�Ϊ�͵�ƽ
		delay(10); //����
		if(!S3){
			if(y<15)
				y++;
			setPointer(2,y);
			while(!S3);
		}
	}
	if(!S4){
		//S2���£�Ϊ�͵�ƽ
		delay(10); //����
		if(!S4){
			if(value < 9)
			{	
				value++;
				numar[y - 12] = value;
			}
			while(!S4);
		}
	}
	if(!S5){
		//S2���£�Ϊ�͵�ƽ
		delay(10); //����
		if(!S5){
			if(value > 0){
				value--;
				numar[y - 12] = value;
			}
			while(!S5);
		}
	}
}

void clear(){
	uint i,j;
	for(i = 0;i<2;i++){
		for(j = 0;j<16;j++)
		{
			setPointer(i+1,j);
			writeData(' ');
		}
	}
	

}
int main(){
	uint answer = 0;
	lcdInit();
	timerInit();
	setPointer(1,0);
	expGenerate('*');
	answer = n1 * n2;
	while(1)
	{
		if(cnt == 3000) //ÿ����Ŀ��ʾ15���ӡ�
		{
			//�����һ��𰸡�
			for(i = 0;i<4;i++)
			{
				num = num * 10 + numar[i];
			}
			//���������
			if(num == answer)
				{
				setPointer(2,0);
				stringSend("RIGHT!");
				}
			else
				{
				setPointer(2,0);
				stringSend("WRONG!");
				}
			num = 0;
			delay(3000);	
			clear();
			setPointer(1,0);
			//��Ӧ��һ��.
			expGenerate('*');
			answer = n1 * n2;
			value = 0;
			cnt = 0;
		}
		keyScan();
	}
	return 0;
}	