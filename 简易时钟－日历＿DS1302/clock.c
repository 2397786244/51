#include <reg52.h>
#include <stdio.h>
//DS1302д�Ĵ��� ��ʼ��ַ
#define REG_W_START 0x80
//DS1302 ���Ĵ��� ��ʼ��ַ
#define REG_R_START 0x81

/*
ʹ��LCD1602��ʾ��ǰ��ʱ��
���°���S2����ʾ��ǰ����������Ϣ(���ڣ���
ʹ��ds1302 RAM������Ϣ��
����S3������ʱ�䵽���ڡ�
*/

//DS1302��������ʹ��P1.0 1.1 1.2���öŰ�����ǰ����
sbit SCLK = P1^0;
sbit IO = P1^1;
sbit CS = P1^2;

//LCD 1602 ����
sbit LCDEN = P3^4;
sbit RS = P3^5; //���ݣ�����ѡ��
sbit RW = P3^6; //���ݶ�д��

//����
sbit S2 = P3^0;
sbit S3 = P3^1;

typedef unsigned char uchar;
typedef unsigned int uint;

//LCD1602 �������� ����ͣ����ݷ��ͣ�ָ��λ�����ã���ʾ����ʼ��
uchar TIME_AR[7] = {
20,50,5,14,7,1,22
};

//����LCD��ʾ״̬
enum STATES {CLOCK,Y_M_D};
static enum STATES S = CLOCK;

void sendTimeToSeries();
//��ʱ����
void delay(uchar time){
	uint i,j;
	for(i = 0;i<114;i++){
		for(j = 0;j < time;j++);
	}
}
//�ж��Ƿ����
void readStates(){
	uchar flag = 0;
	P0 = 0xff; //��P0�Ĵ�������D0-D7�����ߡ�
	RS = 0;  //ָ��ѡ��
	RW = 1;  //��ѡ��
	do{
		LCDEN = 1;  //ʹ��
		flag = P0;  //�����ݿڸ�ֵ��flag���ж�flag�ڰ�λ(D7)�Ƿ�Ϊ0��Ϊ0������˳�ѭ����
		LCDEN = 0;
	}while(flag & 0x80);
}

//��������
void SendCmd(uchar cmd){
	 //ÿ�η���ָ��ǰ�ж������Ƿ����
	readStates();
	RS = 0;
	RW = 0;
	P0 = cmd;
	LCDEN = 1;
	LCDEN = 0;//ָ�����֮�����͡�����ʹ���źŸı����֮������ʱ��涨�����ֵ֮��
}

//���ݷ���
void SendData(uchar dat){
	readStates();
	RS = 1;  //ѡ������
	RW = 0;  //д
	P0 = dat; 
	LCDEN = 1;
	LCDEN = 0;//ʹ�ܱ���150ns
}

//ָ��λ�õ����ã����÷��������
int setPointer(uchar x,uchar y){
	switch(x){
		case 1: SendCmd(0x80 | y);break;
		case 2:	SendCmd(0x80 | 0x40 | y);break;
	}
	return 1;
}

//��ʾ����ʼ��
void lcdInit(){
	SendCmd(0x38);
	SendCmd(0x0c); 
	SendCmd(0x01); //����
	SendCmd(0x06); //����һ��֮��ָ�������ƶ���
}

//�����ַ�������ʾ��������SendData����
void SendString(uchar * str){
	while(*str != '\0'){
		SendData(*str++);
	}
}

//DS1302���� ������ʼ����ʱ���ȡ

//д��һ�ֽ�
void ds1302SendByte(uchar dat){
	uint i;
	for(i = 0;i<8;i++){
		SCLK = 0;
		IO = dat & 0x01;
		SCLK = 1;
		dat >>= 1;
	}
}

//��һ�ֽ�
uchar ds1302ReadByte(){
	uint i;
	uchar dat;
	SCLK = 1;
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

//�������� DS1302�������ݣ��ȷ�������ٷ�������
void DS1302Send(uchar cmd,uchar dat){
	CS = 0;
	SCLK = 0;
	CS = 1;
	ds1302SendByte(cmd);
	ds1302SendByte(dat);
	SCLK = 1; //����ʱ����
	CS = 0;
}

//BCDת��Ϊʮ������
uchar bcd2d(uchar bcd){
	uchar c1,c2;
	c1 = bcd / 16;
	c2 = bcd % 16;
	return c1 * 10 + c2;
}

//ʮ����ת��ΪBCD
uchar d2bcd(uchar d){
	uchar c1,c2;
	c1 = d / 10;
	c2 = d % 10;
	return c1 * 16 + c2; 
}

//��ȡ���� ,DS1302��ȡ���ݣ��ȷ�������ٶ�ȡ����
uchar DS1302Read(uchar cmd){
	uchar dat;
	CS = 0;
	SCLK = 0;
	CS = 1;
	ds1302SendByte(cmd);
	dat = ds1302ReadByte();
	SCLK = 1;
	CS = 0;
	return dat;
}

//DS1302д����,�Ĵ���λ��0x8e
//����д������1�򿪡�
void writeOpen(uchar flag){
	DS1302Send(0x8e,flag);
}

//������ʼ�������������ĳ�ʼֵ��
void DS1302Init(){
	uint i;
	uchar start_addr = REG_W_START;
	writeOpen(0);
	for(i = 0;i<7;i++)
	{
		DS1302Send(start_addr,d2bcd(TIME_AR[i]));
		start_addr+=2;
	}
	writeOpen(1);
}

//�Ĵ���ʱ���ȡ,�������������TIME_AR�С�
void timeGetAndStore()
{
	uint i;
	uchar start_addr = REG_R_START;
	for(i = 0;i<7;i++){
		TIME_AR[i] = bcd2d(DS1302Read(start_addr));
		start_addr+=2;
	}
}

void numberSend(uchar dat){
	uchar c1 = dat / 10;
	uchar c2 = dat % 10;
	SendData(c1 + '0');
	SendData(c2 + '0');
}
//��ʱ����ʾ��LCD�ϣ���ʾСʱ�����ӣ���
void timeDisplay_HMS(){
	//��ȡTIME_AR0-2����ʾ 
	uchar TIME_MSG_LINE1[16] = "CURRENT TIME:";
	timeGetAndStore();
	setPointer(1,0);
	SendString(TIME_MSG_LINE1);
	setPointer(2,0);
	SendString("TIME:");
	numberSend(TIME_AR[2]);
	SendData(':');
	numberSend(TIME_AR[1]);
	SendData(':');
	numberSend(TIME_AR[0]);
	SendData(' ');
}

//ʱ����ʾ���꣬�£��գ���
void timeDisplay_YMD(){
	uchar TIME_MSG_LINE1[16];
	timeGetAndStore();
	sprintf(TIME_MSG_LINE1,"EASY CALENDAR");
	setPointer(1,0);
	SendString(TIME_MSG_LINE1);
	setPointer(2,0);
	SendString("TIME:");
	numberSend(TIME_AR[6]);
	SendData('-');
	numberSend(TIME_AR[4]);
	SendData('-');
	numberSend(TIME_AR[3]);
	SendData(' ');
}

//�������֡�S2�����л�������ʾ��S3��¼ʱ�䡣
void statesAlter(){
	if(!S2){
		delay(10);
		if(!S2){
			switch(S){
				case CLOCK:S = Y_M_D;break;
				case Y_M_D:S = CLOCK;break;
			}
		
		}
		while(!S2); //���ּ��
	}
	if(!S3){
		delay(10);
		if(!S3)
		{
			sendTimeToSeries();
			//����S3���ѵ�ǰʱ�䷢�͵����ڡ�
			while(!S3);
		}
	}
}

void timerInit(){
	TR1 = 1; //��ʱ��1������
	TMOD |= 0x20;
	TH1 = 0xfd;
	TL1 = 0xfd; //������9600
	SM0 = 0;
	SM1 = 1; //����ѡ����ģʽ
}

void sendTimeToSeries()
{
	/*
	���ڷ���
	*/
}

void main()
{
	uint cnt = 0;
	lcdInit();
	DS1302Init();
	timerInit();
	writeOpen(0);
	while(1){
		cnt++;
		if(cnt >= 3000)
		{
			writeOpen(0);
			switch(S)
			{
				case CLOCK: timeDisplay_HMS();break;
				case Y_M_D: timeDisplay_YMD();break;
			}
			cnt = 0;
			writeOpen(1);
		}
		statesAlter();
	}
}