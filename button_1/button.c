#include<reg52.h>


sbit WE = P2^7;
sbit DU = P2^6;

sbit S2 =  P3^0;
sbit BEEP = P2^3;

void delay(int n){
	int i,j;
	for(i=0;i<n;i++){
		for(j=0;j<114;j++);
	}
}

unsigned int word[5] = {0x3f,0x06,0x5b,0x4f,0x66}; //��0-4������
int main(){
	int num1=0;

	while(1){
		P0 = 0xff;
		WE = 1;
		P0 = 0xfe; //����һ�������
		WE = 0;
		if(!S2)
		{
			delay(5); //����
			num1++; 
			if(num1==5)num1=0;
			while(!S2); //���ּ��
		}
		DU = 1;
		P0 = word[num1]; //��ʾһ������                  
		DU = 0;
		delay(5);
		
	}//һ���������ʾ0-4�����֡�
	//��S3���Ƶڶ����������ʾ���� ?��ô��
	return 0;
}