/*дһ���ֽ����ݣ�����BCD��
ת�� �رգ���д������
���������Ĵ�������ʼ��ʾʱ�䣩����ȡʱ�������� LCD ��ʾ����*/
#include <reg52.h>
#define READ_START_ADDR 0x81

sbit SCLK = P1^1;
sbit IO = P1^2;
sbit CE = P1^3;
sbit RS = P3^5; //���ݣ������
sbit RW = P3^6; //����д
sbit EN = P3^4; //ʹ�ܶ�
typedef unsigned char uchar;
typedef unsigned int uint;

uchar CALENDAR_DATA[7] = {20,50,5,14,7,1,22};

void sendByte(uchar dat){
	uint i;
	for(i = 0;i<8;i++){
		SCLK = 0;
		IO = dat & 0x01;
		SCLK = 1;
		dat >>= 1;
	}
}

uchar readByte(){
	uint i;
	uchar dat;
	for(i = 0;i<8;i++){
		SCLK = 0;
		dat >>= 1;		
		if(IO){
			dat |= 0x80;
		}
		SCLK = 1;	
	}
	return dat;
}

//һ�ֽ����ݶ�ȡ
uchar sRead(uchar cmd){
	uchar dat;
	uint i;
	CE = 0;
	SCLK = 0;
	CE = 1; //�����ض�ȡ����
	sendByte(cmd);
	dat = readByte();
	return dat;
}

//һ�ֽ�����д��
void sWrite(uchar cmd,uchar dat)
{
	uint i;
	CE = 0;
	SCLK = 0;
	CE = 1; //�������ؿ�ʼд������
	sendByte(cmd);
	sendByte(dat);
	SCLK = 1; //�����ͷ�����
	CE = 0;
}

//bcd��ԭ
uchar bcdToD(uchar bcd){
	uchar dat1,dat2;
	dat2 = bcd  % 16; //��λ
	dat1 = bcd / 16; //ʮλ
	return dat1 * 10 + dat2;
}

//ת��ΪBCD�롣
uchar dToBCD(uchar dat){
	uchar bcd;
	bcd = dat / 10 * 16 + dat % 10; 
	return bcd;
}

//����д������1�򿪡�
void writeOpen(uint flag){
	if(flag){
		//�ڰ˿�Ĵ������á�
		sWrite(0x8e,1);
	}
	else{
		sWrite(0x8e,0); //�ر�д����
	}
}

void calendarInit(){
	//���ó�ʼʱ�䡣д�����رա�
	//����ʱ��Ϊ2022 7 14  5:50:20
	uchar addr = 0x80;
	uint i;
	writeOpen(0);
	for(i = 0;i<7;i++){
		sWrite(addr,dToBCD(CALENDAR_DATA[i]));
		addr+=2;
	}
	writeOpen(1);
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

void lcdWriteCmd(uchar cmd){
	//ÿ�η���ָ��ǰ�ж������Ƿ����
	readStates();
	RS = 0;
	RW = 0;
	P0 = cmd;
	EN = 1;
	EN = 0;//ָ�����֮�����͡�����ʹ���źŸı����֮������ʱ��涨�����ֵ֮��
}	

void lcdWriteData(uchar dat){
	readStates();
	RS = 1;  //ѡ������
	RW = 0;  //д
	P0 = dat; 
	EN = 1;
	EN = 0;//ʹ�ܱ���150ns
}

void lcdWriteString(uchar dat)
{
	lcdWriteData('0' + dat / 10);
	lcdWriteData('0' + dat % 10);
}

void lcdInit(){
	lcdWriteCmd(0x38);
	lcdWriteCmd(0x0c);
	lcdWriteCmd(0x06);
	lcdWriteCmd(0x01);
}

void calendarRead(){
	uint i;
	uchar addr = READ_START_ADDR;
	for(i = 0;i<7;i++){
		CALENDAR_DATA[i] = bcdToD(sRead(addr));
		addr += 2;
	}
}
void lcdSet(){
	//�������գ�Сʱ�����ӣ�����ʾ��LCD�ϡ�
	lcdWriteCmd(0x80);	
	lcdWriteString(CALENDAR_DATA[6]); //year
	lcdWriteCmd(0x82);
	lcdWriteString(CALENDAR_DATA[5]); //week
	lcdWriteCmd(0x84);
	lcdWriteString(CALENDAR_DATA[4]); //month
	lcdWriteCmd(0x86);
	lcdWriteString(CALENDAR_DATA[3]); // day
	lcdWriteCmd(0x80 | 0x40);
	lcdWriteString(CALENDAR_DATA[2]); //hour
	lcdWriteCmd(0x80 | 0x42);
	lcdWriteString(CALENDAR_DATA[1]); //minute
	lcdWriteCmd(0x80 | 0x44);
	lcdWriteString(CALENDAR_DATA[0]);
}

void main(){
	uint i;
	calendarInit();
	lcdInit();
	while(1)
	{
		writeOpen(0);
		calendarRead();
		lcdSet();
		for(i = 0;i<1000;i++);
		writeOpen(1); //ʱ�ӿ�ʼ��2022��7��14�� 5��50��20�뿪ʼ��ʱ��
	}
}	


