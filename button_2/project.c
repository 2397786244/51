#include<reg52.h>

sbit WE = P2^7;
sbit DU = P2^6;
sbit S2 = P3^0;

unsigned int word[10] = {0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,
                        0x07,0x7f,0x6f};
void display(int num);
void delay(int n){
	int i,j;
	for(i=0;i<n;i++)
	{
	for(j=0;j<114;j++);
	}
}
void main(){
	int num=0;
	while(1)
	{
		
		display(num);
		if(!S2)
			{
			delay(10); //按键消抖
			if(!S2)
				{
				num++;
				if(num==41)num=0;
			}
			while(!S2); //松手检测
			}
	}
}

void display(int num){
			
	int ge,shi;
			shi = num/10;
			ge = num%10;
			//然后在数码管一二上显示这两个数
			P0 = 0xff;
			WE = 1;
			P0 = ~0x01;
			WE = 0;
	
			DU = 1;
			P0 = word[shi];
			DU = 0;
			delay(10);
	
			P0 = 0xff;
			WE = 1;
			P0 = ~0x02;
			WE = 0;

			DU = 1;
			P0 = word[ge];
			DU = 0;
			delay(10);
}