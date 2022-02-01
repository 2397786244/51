/*����ͨ�ŵ�Ӧ�ã�����0���Ƶ�Ƭ���ر�LED�ơ�����1��LED��*/
#include <reg52.h>

typedef unsigned int uint;
typedef unsigned char uchar;

uint getCmd = 0;
uchar * msgOn = "LED�Ѿ��򿪣�";
uchar * msgOff = "LED�Ѿ��رգ�";

void send_string(uchar * str);

//΢�뼶��ʱ
void delay(uint n){
	uint i,j;
	for(i=0;i<n;i++)
		for(j=0;j<114;j++);
}

//��ʱ��1�ĳ�ʼ����
void timer1Init(){
	//��TL1��TH1��ֵΪ0xfd;
	//0xfd: ʹ��11.0592mhz�ĵ�Ƭ��������Ϊ9600
	EA = 1; //�ж��ܿ���λ
	TMOD = 0x20; //����Ϊ��λ�Զ���װģʽ
	TL1 = 0xfd;
	TH1 = 0xfd;
	TR1 = 1; //��ʱ��������
}
void uartInit(){
	ES = 1; //�����ж�
	SM0 = 0;
	SM1 = 1; //ģʽ2.
	REN = 1;//�򿪴���ͨ�Ž��ա�
}

void uartINT() interrupt 4{
		//�����жϡ��ڽ��յ����Է��͵���Ϣ��RI = 0;
	if(RI){
		RI = 0; //���յ���֡��Ȼ��RIҪ������㡣
		getCmd = SBUF; //����
		if(getCmd){
			P1 = 0;//�������е�LED�ơ�			
			send_string(msgOn);
			send_string("\r\n");
			delay(1000);
		}
		else{
			P1 = 0xff;
			send_string(msgOff);
			send_string("\r\n");
			delay(1000);
		}
	}
}	

void main(){
	timer1Init();
	uartInit(); //��ʼ����
	while(1){
	}	
}

void send_string(uchar * str){
	while(*str!='\0'){
		SBUF = *str; //ÿ�η���һ���ֽڡ�
		while(!TI); //�ȴ�������ɡ�
		TI = 0;
		str++;
	}		
}