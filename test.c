#include <stdio.h>
#include <stdlib.h>

int
main()
{
	int a = 12345;
	int b = 12345;
	int c = 12345;
	int d = 12345;
	int e = 12345;
	
	a = 11 + 22;
	b = 44 - 33;
	c = 12 * 21;
	d = 24 / 2;

	e = a + b;
	e = e + c;
	e = e + d;
	
	printf("e=%d\n", e);
	return 0;
}
