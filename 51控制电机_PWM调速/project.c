#include <reg52.h>
/*PWM ����ֱ����� ����*/

sbit S2=P3^0;  	//	���¶�������S2��ת
sbit S3=P3^1;	//	���¶�������S3��ת
sbit S4=P3^2;	//	���¶�������S4 ����
sbit S5=P3^3;   // ����

sbit AIN = P1^0;
sbit BIN = P1^1;
typedef unsigned char uchar;
typedef unsigned int uint;

enum STATES {S,N};
enum STATES NOW_STATES = S; //Ĭ��˳ʱ�롣
uchar pwm_length = 120; //�ñ���������0-170  (�͵�ƽ 0 - pwm_length,�ߵ�ƽpwm_length-255
uchar pwm = 0;

void timerInit(){
	//��ʱ��0��ʼ��������PWM �����ٶȡ�
	EA = 1;
	TMOD = 0x02;  //��λ��װģʽ��
	TH0 = TL0 = 220;  //11.0592MHZ�£��ۼӵ�256�����Ƶ��Ϊ100HZ��(256 - 220) * 1.085 * 255 * 0.001 * 0x001 (��λ��s)
	TR0 = 1;
	ET0 = 1;
}

void pwmGenerate() interrupt 1{
	pwm++;
	if(pwm == 255)
	{
		AIN = 1;
		BIN = 1;
	}
	if(pwm_length == pwm){
		switch(NOW_STATES){
			case S:AIN = 1;BIN = 0;break; //��AIN����͵�ƽ��
			case N:AIN = 0;BIN = 1;break;
		}
	}
}

void delay(unsigned int z)//���뼶��ʱ
{
	unsigned int x,y;
	for(x = z; x > 0; x--)
		for(y = 114; y > 0 ; y--);
}

int main(){
	timerInit();
	while(1){
		if(!S2){
			delay(5);
			if(!S2){
				if(NOW_STATES != S)
					NOW_STATES = S;
			}
			while(!S2);
		}
		if(!S3){
			delay(5);
			if(!S3){
				if(NOW_STATES != N)
					NOW_STATES = N;
			}
			while(!S3);
		}
		if(!S4){
			delay(5);
			if(!S4){ //S4 ������������ߵ�ƽ��ʱ�����--�ߵ�ƽ������������Ϊ�͵�ƽ����
				if(pwm_length < 170)
					pwm_length++;
			}
		}
		if(!S5){
			delay(5);
			if(!S5){
				if(pwm_length > 0)
					pwm_length--;
			}
		}
	}
}