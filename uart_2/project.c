/*���ڷ��ͺ���*/
#include<reg52.h>

typedef unsigned int uint;
typedef unsigned char uchar;
uchar words[2][20]={"���","��ӭʹ�õ�Ƭ����"};
uint i;

void delay(uint n){
	uint i,j;
	for(i=0;i<n;i++){
		for(j=0;j<114;j++);
	}
}	

void uartInit(){
	TR1 = 1; //��ʱ��1������
	TMOD |= 0x20;
	TH1 = 0xfd;
	TL1 = 0xfd; //������9600
	SM0 = 0;
	SM1 = 1; //����ѡ����ģʽ
}

void ch_puts(uchar * str){
	while(*str!='\0'){
		//һ��һ���ķ��ͺ���
		SBUF = *str;
		while(!TI);//�ȴ�������ϡ�TI��1.
		TI = 0;	 //�������
		*str++;
	}
	*str = 0;
}	
int main(){
	uartInit();
	while(1){
		for(i=0;i<2;i++){
			//TI = 1;������TIΪ1����puts��ʱ��������ѭ��
			//puts(words[i]); 
			ch_puts(words[i]);
			ch_puts("\r\n");
			delay(1000);
		}
	}	

}