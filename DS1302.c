#include <REGX52.H>

sbit DS1302_SCLK = P3^6;
sbit DS1302_IO = P3^4;
sbit DS1302_CE = P3^5;

#define DS1302_SEC 0x80
#define DS1302_MIN 0x82
#define DS1302_HR  0x84
#define DS1302_DATE 0x86
#define DS1302_MON 0x88
#define DS1302_DAY 0x8A
#define DS1302_YEAR 0x8C
#define DS1302_WP 0x8E//写保护

extern char DS1302_Time[] = {24,4,4,17,20,0,4,-1};
//要判断小于0所以用了有符号的

void DS1302_Init()
{
	DS1302_CE = 0;
	DS1302_SCLK = 0;
}

void DS1302_WriteByte(unsigned char Command,Data)
{
	unsigned char i;
	DS1302_CE = 1;
	for(i = 0; i < 8 ; i++)
	{
		DS1302_IO = Command & (0x01<<i);
		DS1302_SCLK = 0;
		DS1302_SCLK = 1;
		
		
	}
	for(i = 0; i < 8 ; i++)
	{
		DS1302_IO = Data & (0x01<<i);
		DS1302_SCLK = 0;
		DS1302_SCLK = 1;
		
		
	}
}

unsigned char DS1302_ReadByte(unsigned char Command)
{
	unsigned char i, Data = 0x00;
	Command |= 0x01;//加这句时所用的指令和写入一样即可
	DS1302_CE = 1;
	for(i = 0; i < 8 ; i++)
	{
		DS1302_IO = Command  & (0x01<<i);
		DS1302_SCLK = 0;
		DS1302_SCLK = 1;
		
		
	}
	for(i = 0; i < 8 ; i++)
	{
		DS1302_SCLK=1;
		DS1302_SCLK=0;
		if(DS1302_IO)
			Data |= (0x01<<i);
	}
	DS1302_CE = 0;
	DS1302_IO = 0;
	return Data;
}

void DS1302_SetTime()
{
	DS1302_WriteByte(DS1302_WP,0x00);
	DS1302_WriteByte(DS1302_YEAR,(DS1302_Time[0]/10*16+DS1302_Time[0]%10));
	DS1302_WriteByte(DS1302_MON,(DS1302_Time[1]/10*16+DS1302_Time[1]%10));
	DS1302_WriteByte(DS1302_DATE,(DS1302_Time[2]/10*16+DS1302_Time[2]%10));
	DS1302_WriteByte(DS1302_HR,(DS1302_Time[3]/10*16+DS1302_Time[3]%10));
	DS1302_WriteByte(DS1302_MIN,(DS1302_Time[4]/10*16+DS1302_Time[4]%10));
	DS1302_WriteByte(DS1302_SEC,(DS1302_Time[5]/10*16+DS1302_Time[5]%10));
	DS1302_WriteByte(DS1302_DAY,(DS1302_Time[6]/10*16+DS1302_Time[6]%10));
	DS1302_WriteByte(DS1302_WP,0x80);//最高位是WP位
}
void DS1302_ReadTime()
{
	unsigned char temp;
	temp = DS1302_ReadByte(DS1302_YEAR);
	DS1302_Time[0] = temp/16*10+temp%16;
	temp = DS1302_ReadByte(DS1302_MON);
	DS1302_Time[1] = temp/16*10+temp%16;
	temp = DS1302_ReadByte(DS1302_DATE);
	DS1302_Time[2] = temp/16*10+temp%16;
	temp = DS1302_ReadByte(DS1302_HR);
	DS1302_Time[3] = temp/16*10+temp%16;
	temp = DS1302_ReadByte(DS1302_MIN);
	DS1302_Time[4] = temp/16*10+temp%16;
	temp = DS1302_ReadByte(DS1302_SEC);
	DS1302_Time[5] = temp/16*10+temp%16;
	temp = DS1302_ReadByte(DS1302_DAY);
	DS1302_Time[6] = temp/16*10+temp%16;
	
}
