#include <reg52.h>

typedef unsigned char uchar;
typedef unsigned int uint;

sbit RS = P3^5; //���ݣ������
sbit RW = P3^6; //����д
sbit EN = P3^4; //ʹ�ܶ�

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
int main(){
	uint i = 0,j = 0;
	//������ʾָ�
	writeCmd(0x38);	
	writeCmd(0x0c);
	//����ʾָ��
	writeCmd(0x01); //����ָ��
	//����ָ������
	while(1){
		//дһ���ַ���ָ������+1.
		writeCmd(0x06);
		writeCmd(0x80 | i); //�ӵ�һ������࿪ʼ�ƶ���
		writeData('b');
		writeData('i');
		writeData('g');
		writeData('D');
		writeData('r');
		writeData('e');
		writeData('a');
		writeData('m');
		writeData('e');
		writeData('r');
		delay(300);
		writeCmd(0x80 | 0x40 | (15 - j)); //�ӵڶ������Ҳ������ƶ���
		writeData('W');
		writeData('C');
		writeData('Z');
		delay(300);
		writeCmd(0x01); //����ָ�
		i++;
		j++;
		if(i == 16)
			i = 0; //�����ƶ���ʾbigDreamer.
		if(j == 16)
			j = 0; //��ʾWCZ
	}
	return 0;
}
