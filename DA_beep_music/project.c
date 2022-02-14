#include <reg52.h>
#include <intrins.h>

typedef unsigned char uchar;
typedef unsigned int uint;
//将蜂鸣器IO口接在单片机3.0口上。
sbit musicBeep = P3^0;
//储存低音6到高音615个音符的高位，低位值，用于定时器的设定                                                                
uchar code musicNotesH[15] = {0xF7,0xf8,0xf9,0xf9,0xfa
,0xfa,0xfb,0xfb,0xfc,0xfc,0xfc,0xfd,0xfd,0xfd,0xfd};  
uchar code musicNotesL[15] = {0xDB,0xbe,0x22,0xe1,0x8c,0xdc,0x6c,0xec,
	0x5e,0x93,0xf2,0x47,0x6e,0xb6,0xf6};  

//计算公式: cnt = (1/hz)*10e6 /2 / CLOCK
// h = (65536 - cnt)/256.l为取模 256
uchar musicNextH = 0;
uchar musicNextL = 0; //赋值给TH0和TL0。
//每首歌之间的延时。
void delay(uint n){
	uint i,j;
	for(i=0;i<n;i++)
		for(j=0;j<114;j++);
}
//播放每个音符。
void notesplay(uint n){
	uint i,j;
	TR0 = 1; 
	for(i = 0 ;i<n;i++)
		for(j=0;j<4000;j++);
	TR0 = 0;
}	
//播放一个休止符,n控制时间，单位0.5个半拍
void notes0play(uint n)
{
	uint i,j;		
	for(i = 0 ;i<n;i++)
		for(j=0;j<4000;j++);
}
	
//定时器中断初始化
void timer0Init(){
	EA = 1;
	ET0 = 1;
	TMOD = 0x01;
}	

void beepVoice() interrupt 1{
	TR0 = 0;
	musicBeep = !musicBeep;
	TH0 = musicNextH;
	TL0 = musicNextL;
	TR0 = 1;
}
//把音符转换为musicNotesH和L数组的索引
uint getNotesIndex(int note){
	uint index;
	if(note == -6 || note == -7){
		index = note + 6;
	}
	else{
		index = note + 1;
	}
	return index;
}

void main(){
	uint i;  //循环变量
	uint index;
	uint delay0time = 0;
	uint code music[] = {12,4, 9,1,9,1, 10,2, 10,2, 12,1, 12,1, 0,1, 10,2,10,2, 9,1, 9,1,10,2, 10,2,12,1, 12,2, 
0,1, 9,1, 9,1, 10,2,10,2, 12,1, 12,1, 13,2, 9,1, 9,1, 10,2, 10,1, 10,1, 9,1, 8,1, 8,1,6,1, 6,1, 9,3, 0,4, 0,2, 10,1, 10,1, 
9,1, 8,1, 6,1, 6,1, 9,3, 0,4,0,4, 12,4, 9,1, 10,2, 12,1,12,1, 0,1, 10,2, 9,1, 10,2, 
12,1,12,1, 0,2, 9,1, 9,1, 10,2, 12,1,12,1, 13,2, 13,2, 9,1,9,1, 10,2, 10,1,10,1, 9,1, 8,1,8,1, 6,1,6,1,
9,3, 9,3, 0,4, 0,3, 12,1, 12,1, 10,1, 11,1, 10,1, 9,1, 9,1, 8,3, -1,-1}; //-6 -7 ....- 1 2 3 4567 8--高音1,9 高音2,10,第二个参数是延时时间 
	timer0Init();
	TR0 = 0;
	while(1){
		i = 0;
		while(music[i] != -1) //用-1表示音乐结束
		{
			if(music[i] != 0)
			{//不是休止符的情况
				index = getNotesIndex(music[i]);
				musicNextL = musicNotesL[index];
				musicNextH = musicNotesH[index];
				notesplay(music[i+1]);
			}
			else{
				delay0time = music[i+1];
			//遇到0即休止符。调用休止符延时函数。
				notes0play(delay0time); //不播放，只延时。
			}
			i += 2;
			delay(200);
		}
		delay(5000);
	}
}	