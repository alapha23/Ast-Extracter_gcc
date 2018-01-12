int foo(void);

int boo(void);

int gua(int a){ return a;}

int request(int, int, int, int);

int main(void)
{
	int a, b, c, d;
	
	a = gua(a) - 1;
	a = -a;
	while(gua(a))
	{
		if(a > 5)
		{
			a += 2;
			break;
		}
		else if(a>3)
		{
			continue;
		}
	}

	return 0;
}
