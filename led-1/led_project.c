/*
*��Ƭ��IO��λ����
*led�ĵ���
*led��˸
*��ˮ��(��ٵ���ˮ��hh)
*/

#include <reg52.h>

sbit led0= P1^0;
sbit led1=P1^1;
sbit led2=P1^2;
sbit led3 =P1^3;
sbit led4=P1^4;
sbit led5 = P1^5;
sbit led6 = P1^6;
sbit led7= P1^7;//����IO��
int i;
void delay(int n)
{
	int i;
	int j;
	for(i=0;i<n;i++){for(j=0;j<100;j++);}
} //�ӳٺ���

void main(){
	/*while(1){
	led2=1;
	led=0;
	delay(300);
	led2=0;
	led = 1;
	delay(300);
	} led��˸*/
	while(1){
		P1=0x01;
	for(i=0;i<8;i++){
	P1 = P1*2;
	delay(600);
	}	
	delay(100);
	}
}