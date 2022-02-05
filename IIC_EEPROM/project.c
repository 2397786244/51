/**
*ģ��IIC���ߣ�EEPROM(����ɲ�дֻ��������) ���淢�͵����ݣ����Խ��ա�
* ���Ͷ�����ݣ��ϵ�������UART����ǲ���д��ġ�
**/
#include<reg52.h>
#include<intrins.h>
#define SADDR 0xA0
typedef unsigned char uchar;
typedef unsigned int uint;

//sbit WE = P2^7;
//sbit DU = P2^6;
sbit SDA = P2^0; //������
sbit SCL = P2^1; //IIC ʱ����

//uchar code table[10] = {0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f};
//uchar code pos[] = {0xfe, 0xfd, 0xfb}; ��λ����λѡ��ʾ��P0������
uchar value[16]; //����EEPROM�ж���������
uchar datSend[16] = {0x20,0xfe,0xfd,0xfb,0x06,0x66,0x6d,0x7d,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f};
bit ACKstatus = 0;
//�ӻ�Ӧ��״̬��

void delay(uint n); //��ʱ����
void delay5us(); //��ʱ5us���ҡ�����IICģ��ͨ��ʱ��
void iicStart(); //����IIC���͡��������� ������ʼ�źŵĺ���
void iicEnd(); //������ֹ�źź���
bit getACK(); //�Ӵӻ���Ӧ��״̬�ĺ����������Ӧ�𣬷���1������0
void sendACK(bit i); //��������Ӧ��״̬�ĺ���
uchar getByte(); //��SDA�������ݣ������ظ���������
void sendByte(uchar dat);  //�Ѳ���datһλһλ�ķ��͸�SDA��
void iicWrite(uchar addr,uchar dat); //��dat���͸��ӻ�(��ַΪSADDR��д����׵�ַΪaddr��ʹ��sendByte����)
uchar iicRead(uchar addr); //�ӵ�ַΪSADDR�ӻ���addr��������(ʹ��getByte����)���ظ���������
//void display(uchar num); ��ʾvalueֵ���ڶ�ʱ��0�жϺ����е���

void delay(uint n){
	uint i,j;
	for(i=0;i<n;i++)
		for(j=0;j<114;j++);
}

void delay5us(){
	_nop_();
}

//��ʼ����.
void iicStart(){
	SCL = 1;
	SDA = 1;
	delay5us(); //����4.7us
	SDA = 0;
	delay5us(); //����4us
}
//����������
void iicEnd(){
	SCL = 0;
	SDA = 0;
	SCL = 1;
	delay5us(); //����4us
	SDA = 1;
	delay5us(); //���ٱ���4.7us ��ֹ֮���SDA���ߣ��ͷ�����
}	

//�����ж��Ƿ�õ���Ӧ��
bit getACK(){
	SCL = 0; //����SCLʱ���ߣ�����ӻ�����SDA
	SCL = 1; //���߶�SDA
	delay5us();
	//��������4us����SDA
	if(SDA){
		SCL = 0;
		return 1;
	}
	else{
		SCL = 0;
		return 0;
	}	
}

//��������Ӧ��
void sendACK(bit i){
	SCL = 0; 
	if(i){
		SDA = 1; //NOACK
	}
	else
	{
		SDA = 0;
	}
	SCL = 1;
	delay5us(); //���ٱ���4us
	SCL = 0;
	SDA = 1; //�ͷ�����
}

//��SDA�ж������ݣ�������dat��
uchar getByte(){
	uint i=0;
	uchar dat=0;
	for(i=0;i<8;i++){
		dat <<= 1;
		SCL = 1; //SCL��1����sda������
		if(SDA){
			dat |= 0x01;
		}
		SCL = 0;
	}	
	return dat;
}

//ͨ��IIC���ߣ���EEPROMд���ݡ����д
void iicWrite(uchar addr,uchar dat){
	//���ȣ�������ʼ�ź�
	iicStart();
	//Ȼ���ʹӻ���ַ���϶�д����
	sendByte(SADDR + 0);  //0xA0��EEPROM�ĵ�ַ��0��д��
	//Ȼ����մӻ���һ��Ӧ��.
	ACKstatus = getACK();
	//����Ҫд����׵�ַ�����256���ֽڡ�
	sendByte(addr);
	ACKstatus = getACK();
	sendByte(dat);
	//����num��EEPROM��
	ACKstatus = getACK();
	iicEnd();
}

//��EEPROM�ж������� ����һ��ֵ
uchar iicRead(uchar addr){
	uchar dat;
	//���ȷ�����ʼ�ź�
	iicStart();
	//���ʹӻ���ַ
	sendByte(SADDR + 0 ); //�ӻ���ַ + д�ķ��򣬴���Ҫ�����׵�ַ
	//���մӻ�һ��Ӧ��
	ACKstatus = getACK();
	//����Ҫ���ĵط����׵�ַ
	sendByte(addr);
	getACK();
	//�ٴη�����ʼ�ź�
	iicStart();
	//���ʹӻ���ַ
	sendByte(SADDR + 1); //��
	ACKstatus = getACK();
	dat = getByte();
	//����֮����һ��ACKӦ��
	sendACK(1);
	iicEnd();
	return dat;
}
//�����ֽڵĵĺ�����
void sendByte(uchar dat){
	uint i;
	for(i=0;i<8;i++){
		SCL = 0; //��ʱ�������ͣ�����SDA�仯��
		if(dat& 0x80){
			SDA = 1;
		}
		else{
			SDA = 0;
		}
		dat <<= 1; //��һ����Ϊ0x40
		SCL = 1; //�ӻ����SDA���ݡ�
	}
	SCL = 0; //����SDA�仯��
	SDA = 1; //�ͷ���������
}
//timer1���ڲ���9600�Ĳ�����
void timerInit(){
	ET1 = 1;
	ES = 1; //�����ж�����λ
	TMOD = 0x20; //��ʱ��1�ð�λ�Զ���װģʽ
	TH1 = 0xfd;
	TL1 = 0xfd;
	TR1 = 1;
}
void uartInit(){
	REN = 1;//�������λ SCON�Ĵ���
	SM0 = 0;
	SM1 = 1;//���ڹ�����ʽ����
	//8λUART�����ʿɱ�
}
	
//uart�������ݵ����Եĺ���
void uartSendByte(uchar dat){
	SBUF = dat;
	while(!TI);//�ȴ�������ɡ����֮��Ӳ����1.
	TI = 0; //�������0
}	

void main(){
	uint i;
	uchar addr;
	timerInit();
	uartInit();
		EA = 0;
		addr = 0;
		//����EEPROMд��8���ֽڵ�����
		//д����ɺ󣬶ϵ磬������¼����EEPROM�ж��ո�д�������
		for(i=0;i<16;i++){
			delay(20);	
		    iicWrite(addr,datSend[i]);
			addr++;
		}
		if(ACKstatus){
			P0 = 0;
		}
		else{
			P0 = 0xff;
		}
		addr = 0;
		//��EEPROM��
		for(i=0;i<16;i++){
			delay(20);
			value[i] = iicRead(addr);
			addr++;
		}
		if(ACKstatus){
			P0 = 0;
		}
		else{
			P0 = 0xff;
		}
		EA = 1;
		//���ͨ��UART���͵�����
		for(i=0;i<16;i++)
			uartSendByte(value[i]);
	while(1);
}	