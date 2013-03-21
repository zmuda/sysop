#define SIZE 800000000
int main()
{
	int k[SIZE];
	int i;

	for(i = 0;i < SIZE	; i++)
	{
		k[i] = 0;
	}
	int res=0;
	for(i = 0;i < SIZE	; i++)
	{
		res+=k[i];
	}

	return 0;
}
