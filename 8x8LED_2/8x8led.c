/*��ʾ ��ã�*/
#include<reg52.h>

sbit DIO = P3^4; //14�Ŵ��������
sbit S_CLK = P3^5; //11�ţ��ṩʱ����
sbit R_CLK = P3^6; //12�ţ����������

unsigned char COL_table[3][8]={{0xFF,0xBB,0xAA,0xAA,0x3B,0xA1,0xD7,0xFF},{0xFF,0xD3,0x1B,0x9B,0x5B,0x00,0xBD,0xB0},
{0xE7,0xE7,0xFF,0xE7,0xE7,0xE7,0xE7,0xE7}}; //�����ַ����ֱ����� �� ��
unsigned char ROW_table[3][8]={{0x00,0x40,0x20,0x10,0x08,0x04,0x02,0x00},{0x00,0x40,0x20,0x10,0x08,0x04,0x02,0x01},
{0x80,0x40,0x00,0x10,0x08,0x04,0x02,0x01}}; //���ߵ�����

void display(unsigned char word){
	//�ú��������С�������
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
		//word�����ƶ�һλ��
	}
}

void main(){
	int i,j,k;
	while(1){
		for(i=0;i<3;i++)
		{
			for(k=0;k<1000;k++){//���������ʾ1000�Ρ�Ҳ���� ��ʱ����
				//���ʹ����ʱ��������ֻ����ʾÿ����һ��
				//���������ֶ�̬��ʾ��
				for(j=0;j<8;j++)
				{
						R_CLK = 0;
					//�����д�������
						display(COL_table[i][j]);//�ȴ������ߵ�����
						display(ROW_table[i][j]);//�ٴ������ߵ�����
						R_CLK = 1; //��12��һ�������أ�����һ�������
				}
			}	
		}
	}
}