/**
*����ܾ�̬��ʾ\��̬��ʾ
**/
#include <reg52.h>
#include <intrins.h>

typedef unsigned int uint;
typedef unsigned char uchar;

sbit DU = P2^6; //����ܶ�ѡ LE�ڡ�1��ʱ�������������˱仯��λѡ������ͬ��
sbit WE = P2^7; //�����λѡ

void delay(uint n)
	{
	int i,j;
	for(i=0;i<n;i++)
		{
		for(j=0;j<114;j++);
		}
	}
void main(){
	
	/*WE= 1; //��λѡ������
	P0 = 0xFE;  //1111 1110 ѡ���һ������ܴ�
	WE = 0; //�ر�λѡ������ 
	
	DU = 1;
	P0 = 0x06;
	DU = 0;*/
	//���ƶ�ѡ����ѡҪ��ߵ�ƽ�������ö�����ܡ� 
	//��ѡ�������ر�
	while(1){
		
		P0 = 0xFF; //�������
		WE = 1;
		P0 = 0xFE; 	//��һ������
		WE = 0;
		
		DU = 1;
		P0 = 0x06;
		DU = 0; //��ʾ1
		
		delay(10);
		
		P0 = 0xFF;	//�������	
		WE = 1;
		P0 = 0XFD; //ѡ��ڶ�������
		WE = 0;
		
		DU = 1;
		P0 = 0x5B; //����һ��2������
		DU = 0;
		delay(3);
		
		//�ڵ�������������ʾ3
		P0 = 0XFF;
		WE = 1;
		P0 = 0XFB;
		WE = 0;	//1111 1011
		
		DU = 1;
		P0 = 0x4F; //��ʾ3���� abcdg 0100 1111
		DU = 0;
		delay(3);
	}

}