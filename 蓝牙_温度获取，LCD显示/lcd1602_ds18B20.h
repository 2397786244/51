#ifndef LCD1602_DS18B20_H_
#define LCD1602_DS18B20_H_
typedef unsigned char uchar;
typedef unsigned int uint;
//lcd1602�������ֱ��Ƕ�ȡlcd״̬��д���д���ݡ�
void readStates();
void writeCmd(uchar cmd);
void writeData(uchar dat);
void LCD1602Init();
void SendString(uchar * s);
//ds18B20 ��������
bit initCmd();
void oWireWrite(uchar cmd);
uchar readByteFromOWire();
void delay_us(uint n);
void delay(uint);
#endif