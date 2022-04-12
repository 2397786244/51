#include <reg52.h>
/*PWM 控制直流电机 调速*/

sbit S2=P3^0;  	//	按下独立键盘S2正转
sbit S3=P3^1;	//	按下独立键盘S3反转
sbit S4=P3^2;	//	按下独立键盘S4 减速
sbit S5=P3^3;   // 加速

sbit AIN = P1^0;
sbit BIN = P1^1;
typedef unsigned char uchar;
typedef unsigned int uint;

enum STATES {S,N};
enum STATES NOW_STATES = S; //默认顺时针。
uchar pwm_length = 120; //该变量控制在0-170  (低电平 0 - pwm_length,高电平pwm_length-255
uchar pwm = 0;

void timerInit(){
	//定时器0初始化，负责PWM 调节速度。
	EA = 1;
	TMOD = 0x02;  //八位重装模式。
	TH0 = TL0 = 220;  //11.0592MHZ下，累加到256溢出，频率为100HZ。(256 - 220) * 1.085 * 255 * 0.001 * 0x001 (单位：s)
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
			case S:AIN = 1;BIN = 0;break; //即AIN输出低电平。
			case N:AIN = 0;BIN = 1;break;
		}
	}
}

void delay(unsigned int z)//毫秒级延时
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
			if(!S4){ //S4 减慢。（输出高电平的时间减短--高电平对于输出到电机为低电平。）
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