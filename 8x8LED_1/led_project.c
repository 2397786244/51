/**
*@brief
*8*8µãÕóÆÁµÄÏÔÊ¾¡ª1
**/
#include<reg52.h>

//IO¿ÚµÄ¶¨Òå¡£µãÕóÓÃÁËÈý¸öIO¿Ú£¬ÓÃÈý¸öÎ»¶¨Òå
sbit DIO=P3^4;  //14½Å£¬´®ÐÐÊäÈë¡£
sbit S_CLK=P3^5;	//11½Å¡£ÉÏÉýÑØµÄÊ±ºò°ÑDS¶ËµÄÊý¾Ý·Åµ½ÒÆÎ»¼Ä´æÆ÷ÖÐ¡£
sbit R_CLK=P3^6;	//12,Êä³öËø´æÆ÷¡£ÉÏÉýÑØµÄÊ±ºò²¢ÐÐÊä³
void main(){
	//ÊµÏÖµãÁÁÒ»¸öµã
	//ÐÐÏß0x80;
	//ÁÐÏß0xFE;
	//ÏÈ·¢ÁÐÏß¡£ÈÃÁÐÏßµÄÊý¾Ý½øÈëµÚ¶þÆ¬595µÄ¼Ä´æÆ÷¡
	unsigned int i,dat;
	while(1){
	dat=0xfe; //1111 1110.¶ÔÓ¦ÁÐÏßÉÏQ7-Q0
	for(i=0;i<8;i++){
		S_CLK=0;
		R_CLK=0;
		//¸ø11½ÅÒ»¸öÉÏÉýÑØ
		if(dat&0x01){
			DIO = 1; //Î»¼ì²é£¬Èç¹ûdat×îµÍÎ»Îª1£¬ÔòDIO´®ÐÐÊäÈëÒ»¸ö1
		}
		else{
			DIO = 0;
		}
		S_CLK = 1;
		//°ÑÕâ¸öÊý¾ÝËÍµ½¼Ä´æÆ÷
		dat >>= 1; //ÒÆÎ»¡£°Ñ´ÎµÍÎ»·Åµ½×îµÍÎ»ÉÏ
	}
	dat=0x80;
	for(i=0;i<8;i++){
		S_CLK=0;
		R_CLK=0;
		if(dat&0x01){
			DIO = 1; //Î»¼ì²é£¬Èç¹ûdat×îµÍÎ»Îª1£¬ÔòDIO´®ÐÐÊäÈëÒ»¸ö1
		}
		else{
			DIO = 0;
		}
		S_CLK = 1;
		dat >>= 1;
	}
	R_CLK = 1; //¸ø12½ÅÉÏÉýÑØ£¬½øÐÐ²¢ÐÐÊä³ö
	}
}