#include<reg52.h>


sbit WE = P2^7;
sbit DU = P2^6;

sbit S2 =  P3^0;
sbit BEEP = P2^3;

void delay(int n){
	int i,j;
	for(i=0;i<n;i++){
		for(j=0;j<114;j++);
	}
}

unsigned int word[5] = {0x3f,0x06,0x5b,0x4f,0x66}; //从0-4的数字
int main(){
	int num1=0;

	while(1){
		P0 = 0xff;
		WE = 1;
		P0 = 0xfe; //亮第一个数码管
		WE = 0;
		if(!S2)
		{
			delay(5); //消抖
			num1++; 
			if(num1==5)num1=0;
			while(!S2); //松手检测
		}
		DU = 1;
		P0 = word[num1]; //显示一个数字                  
		DU = 0;
		delay(5);
		
	}//一个数码管显示0-4的数字。
	//用S3控制第二个数码管显示数字 ?怎么做
	return 0;
}