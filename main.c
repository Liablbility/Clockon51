#include <REGX52.H>
#include "LCD1602.h"
#include "DS1302.h"
#include "Key.h"
#include "Timer0.h"
#include "Delay.h"

//温度模块
#include "DS18B20.h"

float T;
unsigned char KeyNum,MODE;
unsigned char TimeSetSelect,TimeSetFlash,firstRunFlag = 1;
unsigned char AlarmSetSelect,AlarmFlag,Alarming=1;
char Alarm_Time[]={8,0,0,2,-1};//时 分 状态  再响时间 
char AlarmAgain[2];
char DS1302_TimeBoundry[]={99,12,31,23,59,59,7};
//要判断小于0所以用了有符号的
code unsigned char Alarm_Des[4][7] = 
	{"Hour  \0","Minute\0","State \0","Delay\0"};
code char Alarm_Boundry[]={23,59,1,5};
code unsigned char WeekDay[7][4] = 
	{"Mon","Tue","Wen","Tur","Fri","Sat","Sun"}; 

void TimeShow();
void DateJudge();
void TimeSet();	
void AlarmInit();
void AlarmSet();
void Alarm();

void main(){
	DS18B20_ConvertT();//温度转换
	LCD_Init();
	DS1302_Init();//初始化时钟芯片
	Timer0_Init();//初始化定时器0
	LCD_ShowString(1,1,"Queenie's Clock");
	Delay(1000);
	LCD_ShowString(1,1,"               ");
	T = DS18B20_ReadT();//读取温度
	//DS1302_SetTime();
	AlarmInit();
	while(1)
	{
		KeyNum = Key();
		if(KeyNum == 1)
		{
			if(MODE == 1)//调完后只写入一次
				DS1302_SetTime();
			if(MODE == 2)//调完后只写入一次
				AlarmInit();
			MODE++;
			MODE %= 3;
		}
		switch(MODE)
		{
			case 0:TimeShow();break;
			case 1:TimeSet();break;
			case 2:AlarmSet();break;
		}
	}
}
void Timer0_Routine() interrupt 1
{		
		static unsigned int T0Count,T1Count;
		TL0 = 0x66;
		TH0 = 0xFC;
		T0Count++;
		if(T0Count >= 500)
		{
			T0Count = 0;
			//P2_0 = ~P2_0;//按位取反
			TimeSetFlash = !TimeSetFlash;//逻辑取反
		}
		T1Count++;
		if(T1Count >= 500 && T1Count < 1000)
		{//控制蜂鸣器发出响声
			if(AlarmFlag)
			{
				if(T1Count <= 650 || (T1Count >= 750 && T1Count<= 900))
					P2_0 = 0;
				else
					P2_0 = 1;
			}
		}
		else if(T1Count >= 1500)
		{
			T1Count = 0;
		}
}

void AlarmInit()
{//再响重置
	AlarmAgain[0] = Alarm_Time[0];
	AlarmAgain[1] = Alarm_Time[1];
}
void AlarmSet(){
	LCD_ShowString(1,1,"Alarm's ");
	LCD_ShowString(1,9,Alarm_Des[AlarmSetSelect]);
	LCD_ShowString(2,6,"   ");
	LCD_ShowString(2,12,"   ");
	LCD_ShowString(2,16," ");
	LCD_ShowNum(2,1,Alarm_Time[0],2);
	LCD_ShowNum(2,4,Alarm_Time[1],2);
	LCD_ShowString(2,3,":");
	if(Alarm_Time[2] == 1)
		LCD_ShowString(2,9,"ON ");
	else
		LCD_ShowString(2,9,"OFF");
	LCD_ShowNum(2,15,Alarm_Time[3],1);
	
	if(KeyNum == 2)
	{
		AlarmSetSelect++;
		AlarmSetSelect %= 4;
	}
	if(KeyNum == 3)
	{
		Alarm_Time[AlarmSetSelect]++;
		if(Alarm_Time[AlarmSetSelect] > Alarm_Boundry[AlarmSetSelect])
		{
			Alarm_Time[AlarmSetSelect] = 0;
		if(AlarmSetSelect == 3)
			Alarm_Time[AlarmSetSelect] = 1;
		}
	}
	if(KeyNum == 4)
	{
		Alarm_Time[AlarmSetSelect]--;
		if(Alarm_Time[AlarmSetSelect] < 0 )
			Alarm_Time[AlarmSetSelect] = Alarm_Boundry[AlarmSetSelect];
	}
	if(AlarmSetSelect == 0 && TimeSetFlash == 1)
	{
		LCD_ShowString(2,1,"  ");//空字符应该在if里否则会只显示当前调的那一位
		
	}
	else
	{
		LCD_ShowNum(2,1,Alarm_Time[0],2);
	}
	if(AlarmSetSelect == 1 && TimeSetFlash == 1)
	{
		LCD_ShowString(2,4,"  ");
	}
	else
	{
		LCD_ShowNum(2,4,Alarm_Time[1],2);
	}
	if(AlarmSetSelect == 2 && TimeSetFlash == 1)
		LCD_ShowString(2,9,"   ");
	else{
		if(Alarm_Time[2] == 1)
			LCD_ShowString(2,9,"ON ");
		else
			LCD_ShowString(2,9,"OFF");
	}
	if(AlarmSetSelect == 3 && TimeSetFlash == 1)
		LCD_ShowString(2,15,"  ");
	else
		LCD_ShowNum(2,15,Alarm_Time[3],1);	
}	

void Alarm()
{// AlarmAgain{时 分}
	//Alarm_Time{时 分 状态 再响时间}
	if(DS1302_Time[3] == AlarmAgain[0] && DS1302_Time[4] == AlarmAgain[1])
	{
		AlarmFlag = Alarming * Alarm_Time[2];
		//Alarm_Time 的状态位相乘是否开启
		if(KeyNum == 3)
		{//再响
			AlarmAgain[1] += Alarm_Time[3];
			if(AlarmAgain[1] > Alarm_Boundry[1])
			{
				AlarmAgain[1] -= (Alarm_Boundry[1]+1);
				AlarmAgain[0]++;
				if(AlarmAgain[0] > Alarm_Boundry[0])
					AlarmAgain[0] -= (Alarm_Boundry[0]+1);
			}
		}
		if(KeyNum == 4)
		{//关闭
			Alarming = 0;//按下4后即使时间匹配也不响
			P2_0 = 1;
			AlarmInit();
		}
	}
	else{//重置初始状态
		Alarming = 1;
		AlarmFlag = 0;
		P2_0 = 1;
	}
}

void TimeShow()
{
		unsigned char i;
		DS1302_ReadTime();
		for(i = 0; i < 7;i++)
			if(DS1302_Time[i] > DS1302_TimeBoundry[i])
				DS1302_Time[i] = DS1302_TimeBoundry[i];
		if(firstRunFlag)
			{//如果是第一次读取
				DS1302_SetTime();
				firstRunFlag = 0;
			}	
	  Alarm();//闹钟
		LCD_ShowNum(1,1,DS1302_Time[0],2);
		LCD_ShowNum(1,4,DS1302_Time[1],2);
		LCD_ShowNum(1,7,DS1302_Time[2],2);
		LCD_ShowString(1,3,"-");
		LCD_ShowString(1,6,"-");
		LCD_ShowString(1,9,"  ");
		LCD_ShowString(1,11,WeekDay[DS1302_Time[6]-1]);
		LCD_ShowString(1,14,"  ");
	if(Alarm_Time[2] == 1)//显示闹钟是否开启
			LCD_ShowChar(1,16,'A');
		else
			LCD_ShowChar(1,16,' ');
		LCD_ShowNum(2,1,DS1302_Time[3],2);
		LCD_ShowNum(2,4,DS1302_Time[4],2);
		LCD_ShowNum(2,7,DS1302_Time[5],2);
		LCD_ShowString(2,3,":");
		LCD_ShowString(2,6,":");
		LCD_ShowString(2,9,"  ");

		EA = 0;//先阻止中断
		DS18B20_ConvertT();
		T = DS18B20_ReadT();//读取温度
		EA = 1;
		if(T<0)
			LCD_ShowChar(2,10,'-');
		else if(T == 0)
			LCD_ShowChar(2,10,' ');
		else
			LCD_ShowChar(2,10,'+');
		LCD_ShowNum(2,11,T,2);
		LCD_ShowChar(2,13,'.');
		LCD_ShowNum(2,14,(unsigned long)(T*10)%10,1);
		LCD_ShowChar(2,15,0xDF);//摄氏度的圈
		LCD_ShowChar(2,16,'C');
}

void DateJudge()//判断每月应该有多少天
{
	if(DS1302_Time[1]== 1||DS1302_Time[1]== 3 ||
		 DS1302_Time[1]== 5||DS1302_Time[1]== 7 ||
		 DS1302_Time[1]== 8||DS1302_Time[1]== 10||
		 DS1302_Time[1]== 12)
		DS1302_TimeBoundry[2] = 31;
	else if(DS1302_Time[1] == 4 || DS1302_Time[1] == 6 ||
					DS1302_Time[1] == 9 || DS1302_Time[1] == 11)
					DS1302_TimeBoundry[2] = 30;
	else //二月
	{
		if(DS1302_Time[0] % 4 == 0)//闰年
			//四年一润，百年不润，四百年再润
			DS1302_TimeBoundry[2] = 29;
		else
			DS1302_TimeBoundry[2] = 28;
	}
}

void TimeSet()
{
	if(KeyNum == 2)
	{
		TimeSetSelect++;
		TimeSetSelect %= 7;
		//LCD_ShowNum(2,10,TimeSetSelect,2);
	}
	if(KeyNum == 3)
	{
		DS1302_Time[TimeSetSelect]++;
		if(TimeSetSelect == 1 || TimeSetSelect == 2||TimeSetSelect == 6)
		{//月和日没有0
			DateJudge();
			if(DS1302_Time[TimeSetSelect] > DS1302_TimeBoundry[TimeSetSelect])
				DS1302_Time[TimeSetSelect] = 1;//月和日 星期 没有0
			if(TimeSetSelect == 1)//如果在调月
				{
					DateJudge();
					if(DS1302_Time[2] > DS1302_TimeBoundry[2])
						//判断当前 日 在修改后的月里有没有超出边界
						DS1302_Time[2] = DS1302_TimeBoundry[2];
				}
		}
		else
		{
			if(DS1302_Time[TimeSetSelect] > DS1302_TimeBoundry[TimeSetSelect])
				DS1302_Time[TimeSetSelect] = 0;
		}
	}
	if(KeyNum == 4)
	{
		DS1302_Time[TimeSetSelect]--;
		if(TimeSetSelect == 1 || TimeSetSelect == 2||TimeSetSelect == 6)
		{//月和日没有0
			DateJudge();
			if(DS1302_Time[TimeSetSelect] < 1)//月和日没有0
				DS1302_Time[TimeSetSelect] = DS1302_TimeBoundry[TimeSetSelect];
			if(TimeSetSelect == 1)//如果在调月
			{
				DateJudge();
				if(DS1302_Time[2] > DS1302_TimeBoundry[2])
					//判断当前 日 在修改后的月里有没有超出边界
					DS1302_Time[2] = DS1302_TimeBoundry[2];
			}
		}
		else
		{
			if(DS1302_Time[TimeSetSelect] < 0)
					DS1302_Time[TimeSetSelect] = DS1302_TimeBoundry[TimeSetSelect];
		 }
	}	
	//显示所修改的时间并且让所调的位置进行闪烁
	if(TimeSetSelect == 0 && TimeSetFlash == 1)
		LCD_ShowString(1,1,"  ");//空字符应该在if里否则会只显示当前调的那一位
	else
		LCD_ShowNum(1,1,DS1302_Time[0],2);
	if(TimeSetSelect == 1 && TimeSetFlash == 1)
		LCD_ShowString(1,4,"  ");
	else
		LCD_ShowNum(1,4,DS1302_Time[1],2);
	if(TimeSetSelect == 2 && TimeSetFlash == 1)
		LCD_ShowString(1,7,"  ");
	else
		LCD_ShowNum(1,7,DS1302_Time[2],2);
	if(TimeSetSelect == 3 && TimeSetFlash == 1)
		LCD_ShowString(2,1,"  ");
	else
		LCD_ShowNum(2,1,DS1302_Time[3],2);	
	if(TimeSetSelect == 4 && TimeSetFlash == 1)
		LCD_ShowString(2,4,"  ");
	else
		LCD_ShowNum(2,4,DS1302_Time[4],2);
	if(TimeSetSelect == 5 && TimeSetFlash == 1)
		LCD_ShowString(2,7,"  ");
	else
		LCD_ShowNum(2,7,DS1302_Time[5],2);
	if(TimeSetSelect == 6 && TimeSetFlash == 1)
		LCD_ShowString(1,11,"   ");
	else
		LCD_ShowString(1,11,WeekDay[DS1302_Time[6]-1]);
}