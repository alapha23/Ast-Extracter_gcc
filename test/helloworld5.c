
int foo(void);

int boo(void);

int gua(void);

int request(int, int, int, int);

int main(void)
{
	int a, b, c, d;
	
	gua();
	if(!boo())
	{
		foo();
		if(gua() == -2)
			foo();
		boo();
		gua();
	}
	else
	{
		a = 1;
		request(a, b, d, c-1);
		if(a == gua())
		{
			c = boo();
			foo();		
		}else if(b == gua())
		{
			foo();
			a--;
		}else
		{
			a++;
			b = gua();
		}
	}

	return 0;
}
