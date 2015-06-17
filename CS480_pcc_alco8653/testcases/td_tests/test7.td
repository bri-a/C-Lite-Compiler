
int a[10];

int foo(a)
int a[];
{
	return a[1+a[0]];
}


main()
{
	int x,y;
	int a[100];
	int *b[100];
	int c[2] = {1,0};
	int *d[2] ={&x, &y};
	
	input(c[1], c[0]);
	
	output(c[1], c[0]);	
	
	c[c[0]+c[1]] = 2;
	
	output(*d[1]+*d[0]);
	
	return;
}
