/*
*��ˮ�ơ�������
*01/07
*/
#include <reg52.h>
#include <intrins.h> //_crol_ �ַ���ѭ������ cror ѭ������

typedef unsigned int uint;
typedef unsigned char uchar;
sbit beep = P2^3; //������

void delay(uint n){ //��ʱ����,�����ʱ
	uint i,j;
	for(i=0;i<n;i++)
	{
	for(j=0;j<114;j++);
	}
}

void main(){
	uint i;
	//��ˮ���������Ĺ���
	while(1)
	{
		delay(100);
		for(i=0;i<8;i++)
		{
			beep = ~beep;
			P1 = ~(0x01<<i);  //ÿ������һλ
			delay(200);
		}
		delay(2000);
	//���������
		for(i=0;i<8;i++){
			beep = ~beep;
			P1 = ~(0x80>>i); //ÿ��ѭ������һλ
			delay(200);
		}
		//����һ������֮����һ�·�����
	}
}