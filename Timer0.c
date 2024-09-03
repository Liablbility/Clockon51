#include <REGX52.H>

void Timer0_Init(){
	TMOD = TMOD&0xF0;
	TMOD = TMOD|0x01;
	TF0 = 0;
	TR0 = 1;
	TL0 = 0x66;
	TH0 = 0xFC;
	ET0 = 1;
	EA = 1;
	PT0 = 0;
}
/*
//中断函数
void Timer0_Routine() interrupt 1
{		
		static unsigned int T0Count;
		TL0 = 0x66;
		TH0 = 0xFC;
		T0Count++;
		if(T0Count >= 1000)
		{
			T0Count = 0;
			P2_0 = ~P2_0;
		}
		
}
*/
