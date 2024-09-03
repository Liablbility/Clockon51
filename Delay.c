
void Delay(unsigned int x)	//@12.000MHz
{
	unsigned char data i, j;
	
	while(x>0){
		i = 2;//这两句也要放入循环
		j = 239;
		do{
			while (--j);
		} while (--i);
		x--;
	}

}
void Delay1000ms(void)	//@12.000MHz
{
	unsigned char data i, j, k;

	i = 46;
	j = 153;
	k = 245;
	do
	{
		do
		{
			while (--k);
		} while (--j);
	} while (--i);
}

