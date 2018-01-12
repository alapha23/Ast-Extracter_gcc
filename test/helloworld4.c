int add(int n, int a)
{
	n++;
	a++;
	return n-1-a;
}

void g(void);
void h(void);

int main(void)
{
  int i, j;
  //a = (char)*(b+3);

  int a = i == 2;
  int b = j == 3;
  if (!add(1,1) || b)
    g();
  else
    h();

  if ((i == 2) && (j == 3))
    g();
  else
    h();

  /*switch(*b)
  {
	  case 1:
		  break;
	  case 2:
		  a++;
		  break;
	  case 3:
		  a--;
	  default:
		  a++; 
  }
*/

/*  int d;
  int f;

  a = 22419;
  ++a;
  b = a*a;
  d = 55519 % a;
  f = 77719;
*/

/*  if( a == 0)
  { 
	  a++;
	  b++;
  }else if(a == 1)
  {
  	a--;
	a++;
	a--;
  }
  */


/*  int *c;
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
*/

  return 0;
}
