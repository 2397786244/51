/**
*@brief �¶�ת�� DS18B20
* ���̣���18B20��ʼ��������ROM����ָ��¶�ת��ָ��
* �ٸ�λ��ROM����ָ���ָ��ӼĴ���0�ֽڿ�ʼ�����������ֽڣ���
* �ѽ����ʾ��������ϡ�
**/
#include <reg52.h>
#include <intrins.h>


sbit DQ = P2^2;  //1-wire���ߵ����ݴ�����
sbit WE = P2^7;
sbit DU = P2^6;

typedef unsigned int uint;
typedef unsigned char uchar;

uchar code table[10]={0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f};
uchar code pos[4] = {0xfe,0xfd,0xfb,0xf7};

//΢�뼶��ʱ
void delay_us(uint us){//���뺯����ʹ��11us��ִ��һ�Σ�ʹ��6us���ҡ�
	while(us>0){
		us--;
	}
}	

void delay(uint n)
{
	uint i,j;
	for(i = 0;i<114;i++)
		for(j=0;j<n;j++);
}

bit initCmd(){
	bit flag;
	DQ = 1;
	_nop_(); //һ��ʼΪ�ߵ�ƽ
	DQ = 0;
	delay_us(80);  //����480us����
	DQ = 1;
	delay_us(2); //�ͷ����ߺ󱣳�һ��ʱ�䣬Լ25us���ң��ȴ��������������źš�
	flag = !DQ;  //������ڣ�DQΪ0���ú�������1.	
	delay_us(15);  //����60us����
	DQ = 1;
	_nop_();  //��ʹ�����ߵ�ʱ�򣬶�Ҫ�������ߡ�����ʱ�Ϳ�ʼһ���������ȶ���
	return flag;
}	

void oWireWrite(uchar cmd){
	uint i;
	//������������1us������һ��дʱ��
	//���д��0����������60-120us,���д1����15us֮�����ߡ�����д��֮���1us���١�
	for(i = 0;i<8;i++){
		DQ = 0;
		_nop_(); //����һ��дʱ��
		if(cmd & 0x01){
			//д1ʱ��
			DQ = 1;  //��15us֮����������
		}	
		else{
			DQ = 0;
			//д0ʱ��
		}
		delay_us(10);
		cmd >>= 1;
		DQ = 1; //�ͷ�����
		_nop_();  //����д���ڵļ��ʱ�䡣
	}	
}

uchar readByteFromOWire(){
	uchar dat = 0;
	uchar pos = 0x01;
	uint i;
	for(i=0;i<8;i++){
		DQ = 0;
		_nop_();  //������������1us��������ʱ��
		DQ = 1; //�ͷ�����
		_nop_();  //�����ߵ�ƽ�ȶ���
		if(DQ){//����
			dat = dat | pos; //������1 ,��һ���÷���dat = (DQ << 7) | (dat >> 1)
		}
		//���DQ��0�Ͳ��ù���
		pos <<= 1;
		delay_us(10);  //�������ʱ��һ�ζ���������Ҫ60us���ϡ�
		DQ = 1; //Ȼ���ͷ�����,׼����һ�ζ�
		_nop_();  //���ζ�������ʱ
	}
	return dat;
}	

void display(uint num){
	uint qian,bai,shi,ge; //�������λ����	
	uint i = 0;
	qian = num / 1000; //���qian=0��������λ���������λ��С���㣬�����λ��С����
	bai = num / 100 % 10;
	shi = num /10 % 10;
	ge = num % 10;
	
	//ǧλ��ʾ�����¶ȸ���100�������
	P0 = 0xff;
	WE = 1;
	P0 = pos[i++];
	WE = 0;
		
	DU = 1;
	P0 = table[qian];
	DU = 0;
	delay(2);
	
	//��λ��ʾ
	P0 = 0xff;
	WE = 1;
	P0 = pos[i++];
	WE = 0;
		
	DU = 1;
	P0 = table[bai];
	DU = 0;
	delay(2);
	//ʮλ��ʾ
	P0 = 0xff;
	WE = 1;
	P0 = pos[i++];
	WE = 0;
		
	DU = 1;
	P0 = table[shi] | 0x80;
	DU = 0;
	delay(2);
	
	//��λ��ʾ
	P0 = 0xff;
	WE = 1;
	P0 = pos[i++];
	WE = 0;
		
	DU = 1;
	P0 = table[ge];
	DU = 0;
	delay(2);
}

void main(){
	//bit flag;
	uint temp = 0;
	uint lowT;
	uint highT; //�����¶ȵĸߵ������ֽ�

	//�ȳ�ʼ������
	//�ѼĴ����ھ����޸�Ϊ12λ��
/*	initCmd();
	oWireWrite(0xcc);  //����ROM
	oWireWrite(0x4e);  //д�ݴ���ָ����޸����üĴ����Ȳ���
	oWireWrite(0xff); //����ֵ
	oWireWrite(0x0);  //����ֵ
	oWireWrite(0x7f);  //12λģʽѡ��
	initCmd(); //ÿ�ν���ͨѶ��ʱ��Ҫ��ʼ����ROM����ָ��
	oWireWrite(0xcc);
	oWireWrite(0x48);  //���Ϳ�����eeprom��ָ�*/
	while(1)
	{
		initCmd();
			//���ڵ����
			oWireWrite(0xcc);  //����ROM�Ĳ���ָ��
			oWireWrite(0x44);  //DS18B20���¶�ת��ָ��
			//�����¶�ת��ָ��֮��Ҫ�ٴθ�λ�������������������κ�ͨ��֮ǰ��Ҫ���г�ʼ��
			initCmd();
			//��ȡ�ݴ��������ݣ����ո�ת���õ��¶ȣ�
			oWireWrite(0xcc);  //����ROMָ��
			oWireWrite(0xbe);  //��ȡ�ݴ��������ʱ��18B20�Ὺʼ�����ݴ����ڵ�ֵ��
			lowT = readByteFromOWire();
			highT = readByteFromOWire();  //�ӵ�0���ֽڿ�ʼ��������ֻ��ǰ�����ֽ�
			temp = (highT * 256 ) | lowT;
			temp = (temp * 0.0625 + 0.05 ) * 10;  //�õ��¶�ֵ,�������뱣��һλС��
			//Ȼ�����������ʾtemp��temp���һλΪС�����֡�
			display(temp);		
	}
}	