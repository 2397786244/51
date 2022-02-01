/*串口发送汉字*/
#include<reg52.h>

typedef unsigned int uint;
typedef unsigned char uchar;
uchar words[2][20]={"你好","欢迎使用单片机！"};
uint i;

void delay(uint n){
	uint i,j;
	for(i=0;i<n;i++){
		for(j=0;j<114;j++);
	}
}	

void uartInit(){
	TR1 = 1; //定时器1工作。
	TMOD |= 0x20;
	TH1 = 0xfd;
	TL1 = 0xfd; //波特率9600
	SM0 = 0;
	SM1 = 1; //串口选择工作模式
}

void ch_puts(uchar * str){
	while(*str!='\0'){
		//一个一个的发送汉字
		SBUF = *str;
		while(!TI);//等待传送完毕。TI置1.
		TI = 0;	 //软件置零
		*str++;
	}
	*str = 0;
}	
int main(){
	uartInit();
	while(1){
		for(i=0;i<2;i++){
			//TI = 1;不先置TI为1，在puts的时候会进入死循环
			//puts(words[i]); 
			ch_puts(words[i]);
			ch_puts("\r\n");
			delay(1000);
		}
	}	

}