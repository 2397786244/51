#include "lcd1602_ds18B20.h"
#include <reg52.h>
#include <intrins.h>

//串口的发送(温度发送) 串口的接收(lcd1602内容显示 即：灯已经被打开/关闭,LED流水灯) 
uint flag = 0;

void BlueTooth_uart() interrupt 4{
	if(RI){
		//接收到发送到MCU的命令。
		switch(SBUF){
			case 0x20:flag = 1;SendString("LED OPEN!");break;
			case 0x02:flag = 0;SendString("LED CLOSE!");break;
		}
		RI= 0;
	}
}

void timerInit(){
	EA = 1;
	//定时器1初始化。产生波特率9600	
	PCON &= 0x7F;		
	SCON = 0x50;			
	TMOD &= 0x0F;		
	TMOD |= 0x20;	 //设置八位重装	
	TL1 = 0xFD;		
	TH1 = 0xFD;	//设置初值。
	ET1 = 0;		
	TR1 = 1;
	ES = 1;  //打开串口中断
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
		//判断flag 状态，flag = 1时led打开流水灯。	
		if(flag){
			P1 = 0xfe;
			for(i = 0;i<8;i++){
				P1 = _crol_(P1,1);  //把P1 循环往左移动一位。
				delay(100);
			}
		}
		else{
			P1 = 0xff; //关闭流水灯。
		}
		//实时温度发送。
		if(2000==cnt){
			//温度转换
			initCmd();  
			oWireWrite(0xcc);  //忽略ROM的操作指令
			oWireWrite(0x44);  //DS18B20的温度转换指令
			
			//温度获取
			initCmd();
			oWireWrite(0xcc);  //忽略ROM指令
			oWireWrite(0xbe);  //读取暂存器。这个时候18B20会开始返回暂存器内的值。
			lowT = readByteFromOWire();
			highT = readByteFromOWire();  //从第0个字节开始读。这里只读前两个字节
			temp = (highT * 256 ) | lowT;
			temp = (temp * 0.0625 + 0.05)*10 ;  //得到温度值,四舍五入保留一位小数
			
			//串口发送当前温度
			UartStringSend(temp);
			cnt = 0;
		}
		cnt++;
	}
}