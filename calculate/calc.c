/**
*���ԣ����׼����� +-*����
**/
#include <reg52.h>

typedef unsigned int uint;
typedef unsigned char uchar;

void delay(uint n);
void keyscan();
void display(uint n);
int transform(uint n);
int calcc(uint n1,uint n2,uint type);

//�����λѡ����ѡ
sbit WE = P2^7;
sbit DU = P2^6;

//LED��
sbit led = P1^0;

//���°�����ֵ��
uint value; //0-����1��1-����2��2-����3��4-����4��5-����5��6-����6��8-����7��
//9-����8��10-����9��13-����0��14- =�š�+-3 - - 7, * - 11, ���� 15
//��������û�����õİ�����led��2�롣����Ϊ0������1�롣

uchar calc; //�������
uint table[10] = {0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f};
//��������0-9 

void delay(uint n){
	int i,j;
	for(i=0;i<n;i++){
		for(j=0;j<114;j++);
	}
}
void keyscan(){
	P3 = 0xf0; //��ɨ�� 
	if(P3 != 0xf0)
	{
		//˵��������һ��������
		delay(10);//�������
		if(P3!=0xf0)
		{
			switch(P3)
				{
				case 0xe0 : value = 0;break;
				case 0xd0 : value = 1;break;
				case 0xb0 : value = 2;break;
				case 0x70 : value = 3;break;
				}//�������µ��������	
			P3 = 0x0f;
			//��ɨ��
			switch(P3){
				case 0x0e : value += 0;break;
				case 0x0d : value += 4;break;
				case 0x0b : value += 8;break;
				case 0x07 : value += 12;break;
			}
			while(P3!=0x0f); //���ּ�⡣���м��֮��
		}
	}
}
int transform(uint n){
	uint num=0;
	switch(n)
	{
		case 0:num=1;break;
		case 1:num=2;break;
		case 2:num=3;break;
		case 4:num=4;break;
		case 5:num=5;break;
		case 6:num=6;break;
		case 8:num=7;break;
		case 9:num=8;break;
		case 10:num=9;break;
		case 13:num=0;break;
	}
	return num;
}

int calcc(uint n1,uint n2,uint type)
{
	uint result;
	switch(type)
	{
		case 3: result = n1+n2;break; //n1+n2;
		case 7:
			result = (n1>n2) ? n1-n2 : n2-n1 ;break;
		case 11: result = n1*n2;break;
		
		//������ʱ����
	}
	return result;
}

void display(uint n)
{
	uint shi,ge,i=0;
	shi = n/10;
	ge = n%10;
	while(1)
	{
	if(shi!=0)
	{
		P0 = 0xff;
		WE = 1;
		P0 = ~0x01; //��һ�������
		WE = 0;
		
		DU = 1;
		P0 = table[shi];
		DU = 0; //��ʾʮλ��
		delay(10);
	}
	P0 = 0xff;
	WE = 1;
	P0 = ~0x02;
	WE = 0;
	
	DU = 1;
	P0 = table[ge];
	DU = 0; //��ʾ��λ
	delay(10);
	if(n==0)break;
	i++;
	if(i==114*1000)
		break;
	}
}
void main()
{
	uint n1=0,n2=0,type;
	while(1)
	{
		display(0);
		//��һ������
		P3 = 0xf0;
		while(P3==0xf0);
		keyscan();
		n1 = transform(value);
		//����Ҫ�ȴ����������
		P3 = 0xf0;
		while(P3==0xf0);
		keyscan();
		type = value;
		//�ȴ�����ڶ�������
		P3 = 0xf0;
		while(P3==0xf0);
		keyscan();
		n2 = transform(value);
		display(calcc(n1,n2,type));
	}
}