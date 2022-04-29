#include "lcd1602_ds18B20.h"
#include <reg52.h>
#include <intrins.h>

//���ڵķ���(�¶ȷ���) ���ڵĽ���(lcd1602������ʾ �������Ѿ�����/�ر�,LED��ˮ��) 
uint flag = 0;

void BlueTooth_uart() interrupt 4{
	if(RI){
		//���յ����͵�MCU�����
		switch(SBUF){
			case 0x20:flag = 1;SendString("LED OPEN!");break;
			case 0x02:flag = 0;SendString("LED CLOSE!");break;
		}
		RI= 0;
	}
}

void timerInit(){
	EA = 1;
	//��ʱ��1��ʼ��������������9600	
	PCON &= 0x7F;		
	SCON = 0x50;			
	TMOD &= 0x0F;		
	TMOD |= 0x20;	 //���ð�λ��װ	
	TL1 = 0xFD;		
	TH1 = 0xFD;	//���ó�ֵ��
	ET1 = 0;		
	TR1 = 1;
	ES = 1;  //�򿪴����ж�
	REN = 1;
}

void UartStringSend(uint dat){
	char msg[20] = "Current Temp: ";
	char * ptr;
	uchar bai;
	uchar shi;
	uchar ge;
	bai = dat / 100;
	shi = dat / 10 % 10;
	ge = dat % 10;
	msg[14] = bai + '0';
	msg[15] = shi + '0';
	msg[16] = '.';
	msg[17] = ge  + '0';
	msg[18] = '\0';
	ptr = msg;
	while(*ptr != '\0'){
		SBUF = *ptr;
		while(!TI);
		TI = 0;
		ptr++;
	}
}

void main(){
	uint highT;
	uint lowT;
	uint cnt = 0;
	uint temp;
	uint i;
	LCD1602Init();
	timerInit();
	while(1){
		//�ж�flag ״̬��flag = 1ʱled����ˮ�ơ�	
		if(flag){
			P1 = 0xfe;
			for(i = 0;i<8;i++){
				P1 = _crol_(P1,1);  //��P1 ѭ�������ƶ�һλ��
				delay(100);
			}
		}
		else{
			P1 = 0xff; //�ر���ˮ�ơ�
		}
		//ʵʱ�¶ȷ��͡�
		if(2000==cnt){
			//�¶�ת��
			initCmd();  
			oWireWrite(0xcc);  //����ROM�Ĳ���ָ��
			oWireWrite(0x44);  //DS18B20���¶�ת��ָ��
			
			//�¶Ȼ�ȡ
			initCmd();
			oWireWrite(0xcc);  //����ROMָ��
			oWireWrite(0xbe);  //��ȡ�ݴ��������ʱ��18B20�Ὺʼ�����ݴ����ڵ�ֵ��
			lowT = readByteFromOWire();
			highT = readByteFromOWire();  //�ӵ�0���ֽڿ�ʼ��������ֻ��ǰ�����ֽ�
			temp = (highT * 256 ) | lowT;
			temp = (temp * 0.0625 + 0.05)*10 ;  //�õ��¶�ֵ,�������뱣��һλС��
			
			//���ڷ��͵�ǰ�¶�
			UartStringSend(temp);
			cnt = 0;
		}
		cnt++;
	}
}