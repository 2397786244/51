/*显示 你好！*/
#include<reg52.h>

sbit DIO = P3^4; //14脚串行输入端
sbit S_CLK = P3^5; //11脚，提供时钟线
sbit R_CLK = P3^6; //12脚，输出锁存器

unsigned char COL_table[3][8]={{0xFF,0xBB,0xAA,0xAA,0x3B,0xA1,0xD7,0xFF},{0xFF,0xD3,0x1B,0x9B,0x5B,0x00,0xBD,0xB0},
{0xE7,0xE7,0xFF,0xE7,0xE7,0xE7,0xE7,0xE7}}; //三个字符，分别是你 好 ！
unsigned char ROW_table[3][8]={{0x00,0x40,0x20,0x10,0x08,0x04,0x02,0x00},{0x00,0x40,0x20,0x10,0x08,0x04,0x02,0x01},
{0x80,0x40,0x00,0x10,0x08,0x04,0x02,0x01}}; //行线的数据

void display(unsigned char word){
	//该函数传入列、行数据
	int i;
	for(i=0;i<8;i++){
		S_CLK = 0;
		if(word&0x01){
			DIO = 1;
		}
		else{
			DIO = 0;
		}
		S_CLK = 1;
		word >>= 1;
		//word向右移动一位。
	}
}

void main(){
	int i,j,k;
	while(1){
		for(i=0;i<3;i++)
		{
			for(k=0;k<1000;k++){//把这个字显示1000次。也是有 延时作用
				//如果使用延时函数，则只会显示每个字一行
				//共三个数字动态显示。
				for(j=0;j<8;j++)
				{
						R_CLK = 0;
					//共八行传入数据
						display(COL_table[i][j]);//先传入列线的数据
						display(ROW_table[i][j]);//再传入行线的数据
						R_CLK = 1; //给12脚一个上升沿，把这一行输出。
				}
			}	
		}
	}
}