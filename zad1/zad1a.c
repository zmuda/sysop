#include <stdio.h>
#include <stdlib.h>

int func() 
{
	double d = 0;
	int i = 0;
	for(i = 0 ; i < 30000000 ; i++)
	{
		d = 2 * 7.5;
		d = d / 2;
		d = d * 3;
		d = d / 4;
		d = d * 6 * 7 * 8 / 3.5;
	}
	return 1;
}

int main(int argc, char **argv)
{
	func();	
	func();
	func();

	return 0;
}

