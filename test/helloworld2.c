int add(int n, int a)
{
	return n-1-a;
}


int main(void)
{
  int a;
  
  int b;
  int d;
  int f;

  a = 22419;
  ++a;
  b = a*a;
  d = 55519 % a;
  f = 77719;


  if( a == 0)
  { 
	  a++;
	  b++;
  }else if(a == 1)
  {
  	a--;
	a++;
	a--;
  }


  int *c;
  a = 100;

  do
  {
  	a--;
	a += -5;

  }while(a>0);


  *c = 1;
   *(c+1+a) = 3;
 char* c_a = "abc";
  while(a>0)
  {
//	  printf("test: %d fdfdsa\n", a);
	  a--;
	  a -= 3;
  }


  return 0;
}
