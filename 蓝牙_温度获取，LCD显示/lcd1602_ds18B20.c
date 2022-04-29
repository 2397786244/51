#include "lcd1602_ds18B20.h"
#include <reg52.h>
#include <intrins.h>

sbit RS = P3^5; 
sbit RW = P3^6; 
sbit EN = P3^4; 

sbit DQ = P2^2;  //1-wire���ߵ����ݴ�����
void delay(uint n)
{
	uint i,j;
	for(i = 0;i<114;i++)
		for(j=0;j<n;j++);
}

void delay_us(uint us){//���뺯����ʹ��11us��ִ��һ�Σ�ʹ��6us���ҡ�
	while(us>0){
		us--;
	}
}

//LCD1602��������
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

//DS18B20��������
bit initCmd(){
	bit flag;
	DQ = 1;
	_nop_(); //һ��ʼΪ�ߵ�ƽ
	DQ = 0;
	delay_us(80);  //����480us����
	DQ = 1;
	delay_us(2); //�ͷ����ߺ󱣳�һ��ʱ�䣬Լ25us���ң��ȴ��������������źš�
	flag = !DQ;  //������ڣ�DQΪ0���ú�������1.	
	delay_us(15);  //����60us����
	DQ = 1;
	_nop_();  //��ʹ�����ߵ�ʱ�򣬶�Ҫ�������ߡ�����ʱ�Ϳ�ʼһ���������ȶ���
	return flag;
}	

void oWireWrite(uchar cmd){
	uint i;
	//������������1us������һ��дʱ��
	//���д��0����������60-120us,���д1����15us֮�����ߡ�����д��֮���1us���١�
	for(i = 0;i<8;i++){
		DQ = 0;
		_nop_(); //����һ��дʱ��
		if(cmd & 0x01){
			//д1ʱ��
			DQ = 1;  //��15us֮����������
		}	
		else{
			DQ = 0;
			//д0ʱ��
		}
		delay_us(10);
		cmd >>= 1;
		DQ = 1; //�ͷ�����
		_nop_();  //����д���ڵļ��ʱ�䡣
	}	
}

uchar readByteFromOWire(){
	uchar dat = 0;
	uchar pos = 0x01;
	uint i;
	for(i=0;i<8;i++){
		DQ = 0;
		_nop_();  //������������1us��������ʱ��
		DQ = 1; //�ͷ�����
		_nop_();  //�����ߵ�ƽ�ȶ���
		if(DQ){//����
			dat = dat | pos; //������1 ,��һ���÷���dat = (DQ << 7) | (dat >> 1)
		}
		//���DQ��0�Ͳ��ù���
		pos <<= 1;
		delay_us(10);  //�������ʱ��һ�ζ���������Ҫ60us���ϡ�
		DQ = 1; //Ȼ���ͷ�����,׼����һ�ζ�
		_nop_();  //���ζ�������ʱ
	}
	return dat;
}

void LCD1602Init(){
	//������ʾָ�
	writeCmd(0x38);	
	writeCmd(0x0c);
	//����ʾָ��
	writeCmd(0x01); //����ָ��
	//����ָ������
	writeCmd(0x06);
}

void SendString(uchar * s){
	uchar * ptr = s;
	writeCmd(0x80);
	while(*ptr){
		writeData(*ptr);
		ptr++;
	}
}