#ifndef __DS1302_H__
#define __DS1302_H__

extern char DS1302_Time[];

DS1302_Init();
DS1302_WriteByte(unsigned char Command,Data);
DS1302_ReadByte(unsigned char Command);
void DS1302_SetTime();
void DS1302_ReadTime();

#endif