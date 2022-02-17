/**
*@brief 温度转换 DS18B20
* 流程：对18B20初始化，发送ROM操作指令，温度转换指令
* 再复位，ROM操作指令，读指令（从寄存器0字节开始读，读两个字节）。
* 把结果显示在数码管上。
**/
#include <reg52.h>
#include <intrins.h>


sbit DQ = P2^2;  //1-wire总线的数据传输线
sbit WE = P2^7;
sbit DU = P2^6;

typedef unsigned int uint;
typedef unsigned char uchar;

uchar code table[10]={0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f};
uchar code pos[4] = {0xfe,0xfd,0xfb,0xf7};

//微秒级延时
void delay_us(uint us){//进入函数，使用11us，执行一次，使用6us左右。
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
	_nop_(); //一开始为高电平
	DQ = 0;
	delay_us(80);  //保持480us以上
	DQ = 1;
	delay_us(2); //释放总线后保持一段时间，约25us左右，等待器件发出存在信号。
	flag = !DQ;  //如果存在，DQ为0，让函数返回1.	
	delay_us(15);  //保持60us以上
	DQ = 1;
	_nop_();  //不使用总线的时候，都要把它拉高。给延时和开始一样，让他稳定。
	return flag;
}	

void oWireWrite(uchar cmd){
	uint i;
	//拉低总线至少1us，产生一个写时序。
	//如果写入0，拉低总线60-120us,如果写1，在15us之内拉高。两次写入之间隔1us至少。
	for(i = 0;i<8;i++){
		DQ = 0;
		_nop_(); //产生一个写时序。
		if(cmd & 0x01){
			//写1时序
			DQ = 1;  //在15us之内拉高总线
		}	
		else{
			DQ = 0;
			//写0时序
		}
		delay_us(10);
		cmd >>= 1;
		DQ = 1; //释放总线
		_nop_();  //两次写周期的间隔时间。
	}	
}

uchar readByteFromOWire(){
	uchar dat = 0;
	uchar pos = 0x01;
	uint i;
	for(i=0;i<8;i++){
		DQ = 0;
		_nop_();  //拉低总线至少1us。产生读时序。
		DQ = 1; //释放总线
		_nop_();  //让总线电平稳定。
		if(DQ){//采样
			dat = dat | pos; //读到了1 ,另一种用法：dat = (DQ << 7) | (dat >> 1)
		}
		//如果DQ是0就不用管了
		pos <<= 1;
		delay_us(10);  //读完后延时。一次读周期至少要60us以上。
		DQ = 1; //然后释放总线,准备下一次读
		_nop_();  //两次读周期延时
	}
	return dat;
}	

void display(uint num){
	uint qian,bai,shi,ge; //即最大四位数。	
	uint i = 0;
	qian = num / 1000; //如果qian=0，即是两位数，则第三位加小数点，否则个位加小数点
	bai = num / 100 % 10;
	shi = num /10 % 10;
	ge = num % 10;
	
	//千位显示，即温度高于100的情况。
	P0 = 0xff;
	WE = 1;
	P0 = pos[i++];
	WE = 0;
		
	DU = 1;
	P0 = table[qian];
	DU = 0;
	delay(2);
	
	//百位显示
	P0 = 0xff;
	WE = 1;
	P0 = pos[i++];
	WE = 0;
		
	DU = 1;
	P0 = table[bai];
	DU = 0;
	delay(2);
	//十位显示
	P0 = 0xff;
	WE = 1;
	P0 = pos[i++];
	WE = 0;
		
	DU = 1;
	P0 = table[shi] | 0x80;
	DU = 0;
	delay(2);
	
	//个位显示
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
	uint highT; //储存温度的高低两个字节

	//先初始化器件
	//把寄存器内精度修改为12位。
/*	initCmd();
	oWireWrite(0xcc);  //忽略ROM
	oWireWrite(0x4e);  //写暂存器指令，即修改配置寄存器等部分
	oWireWrite(0xff); //上限值
	oWireWrite(0x0);  //下限值
	oWireWrite(0x7f);  //12位模式选择
	initCmd(); //每次进行通讯的时候都要初始化，ROM操作指令
	oWireWrite(0xcc);
	oWireWrite(0x48);  //发送拷贝到eeprom的指令。*/
	while(1)
	{
		initCmd();
			//存在的情况
			oWireWrite(0xcc);  //忽略ROM的操作指令
			oWireWrite(0x44);  //DS18B20的温度转换指令
			//发送温度转换指令之后，要再次复位。即主机和器件进行任何通信之前都要进行初始化
			initCmd();
			//读取暂存器的内容（即刚刚转换好的温度）
			oWireWrite(0xcc);  //忽略ROM指令
			oWireWrite(0xbe);  //读取暂存器。这个时候18B20会开始返回暂存器内的值。
			lowT = readByteFromOWire();
			highT = readByteFromOWire();  //从第0个字节开始读。这里只读前两个字节
			temp = (highT * 256 ) | lowT;
			temp = (temp * 0.0625 + 0.05 ) * 10;  //得到温度值,四舍五入保留一位小数
			//然后用数码管显示temp。temp最后一位为小数部分。
			display(temp);		
	}
}	