/*串口通信的应用，发送0控制单片机关闭LED灯。发送1打开LED灯*/
#include <reg52.h>

typedef unsigned int uint;
typedef unsigned char uchar;

uint getCmd = 0;
uchar * msgOn = "LED已经打开！";
uchar * msgOff = "LED已经关闭！";

void send_string(uchar * str);

//微秒级延时
void delay(uint n){
	uint i,j;
	for(i=0;i<n;i++)
		for(j=0;j<114;j++);
}

//定时器1的初始化。
void timer1Init(){
	//把TL1和TH1赋值为0xfd;
	//0xfd: 使得11.0592mhz的单片机波特率为9600
	EA = 1; //中断总控制位
	TMOD = 0x20; //设置为八位自动重装模式
	TL1 = 0xfd;
	TH1 = 0xfd;
	TR1 = 1; //定时器启动。
}
void uartInit(){
	ES = 1; //串口中断
	SM0 = 0;
	SM1 = 1; //模式2.
	REN = 1;//打开串口通信接收。
}

void uartINT() interrupt 4{
		//串口中断。在接收到电脑发送的信息后将RI = 0;
	if(RI){
		RI = 0; //接收到的帧，然后RI要软件清零。
		getCmd = SBUF; //接收
		if(getCmd){
			P1 = 0;//点亮所有的LED灯。			
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
	uartInit(); //初始化。
	while(1){
	}	
}

void send_string(uchar * str){
	while(*str!='\0'){
		SBUF = *str; //每次发送一个字节。
		while(!TI); //等待发送完成。
		TI = 0;
		str++;
	}		
}