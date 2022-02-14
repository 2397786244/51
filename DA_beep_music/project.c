#include <reg52.h>
#include <intrins.h>

typedef unsigned char uchar;
typedef unsigned int uint;
//��������IO�ڽ��ڵ�Ƭ��3.0���ϡ�
sbit musicBeep = P3^0;
//�������6������615�������ĸ�λ����λֵ�����ڶ�ʱ�����趨                                                                
uchar code musicNotesH[15] = {0xF7,0xf8,0xf9,0xf9,0xfa
,0xfa,0xfb,0xfb,0xfc,0xfc,0xfc,0xfd,0xfd,0xfd,0xfd};  
uchar code musicNotesL[15] = {0xDB,0xbe,0x22,0xe1,0x8c,0xdc,0x6c,0xec,
	0x5e,0x93,0xf2,0x47,0x6e,0xb6,0xf6};  

//���㹫ʽ: cnt = (1/hz)*10e6 /2 / CLOCK
// h = (65536 - cnt)/256.lΪȡģ 256
uchar musicNextH = 0;
uchar musicNextL = 0; //��ֵ��TH0��TL0��
//ÿ�׸�֮�����ʱ��
void delay(uint n){
	uint i,j;
	for(i=0;i<n;i++)
		for(j=0;j<114;j++);
}
//����ÿ��������
void notesplay(uint n){
	uint i,j;
	TR0 = 1; 
	for(i = 0 ;i<n;i++)
		for(j=0;j<4000;j++);
	TR0 = 0;
}	
//����һ����ֹ��,n����ʱ�䣬��λ0.5������
void notes0play(uint n)
{
	uint i,j;		
	for(i = 0 ;i<n;i++)
		for(j=0;j<4000;j++);
}
	
//��ʱ���жϳ�ʼ��
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
//������ת��ΪmusicNotesH��L���������
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
	uint i;  //ѭ������
	uint index;
	uint delay0time = 0;
	uint code music[] = {12,4, 9,1,9,1, 10,2, 10,2, 12,1, 12,1, 0,1, 10,2,10,2, 9,1, 9,1,10,2, 10,2,12,1, 12,2, 
0,1, 9,1, 9,1, 10,2,10,2, 12,1, 12,1, 13,2, 9,1, 9,1, 10,2, 10,1, 10,1, 9,1, 8,1, 8,1,6,1, 6,1, 9,3, 0,4, 0,2, 10,1, 10,1, 
9,1, 8,1, 6,1, 6,1, 9,3, 0,4,0,4, 12,4, 9,1, 10,2, 12,1,12,1, 0,1, 10,2, 9,1, 10,2, 
12,1,12,1, 0,2, 9,1, 9,1, 10,2, 12,1,12,1, 13,2, 13,2, 9,1,9,1, 10,2, 10,1,10,1, 9,1, 8,1,8,1, 6,1,6,1,
9,3, 9,3, 0,4, 0,3, 12,1, 12,1, 10,1, 11,1, 10,1, 9,1, 9,1, 8,3, -1,-1}; //-6 -7 ....- 1 2 3 4567 8--����1,9 ����2,10,�ڶ�����������ʱʱ�� 
	timer0Init();
	TR0 = 0;
	while(1){
		i = 0;
		while(music[i] != -1) //��-1��ʾ���ֽ���
		{
			if(music[i] != 0)
			{//������ֹ�������
				index = getNotesIndex(music[i]);
				musicNextL = musicNotesL[index];
				musicNextH = musicNotesH[index];
				notesplay(music[i+1]);
			}
			else{
				delay0time = music[i+1];
			//����0����ֹ����������ֹ����ʱ������
				notes0play(delay0time); //�����ţ�ֻ��ʱ��
			}
			i += 2;
			delay(200);
		}
		delay(5000);
	}
}	