 #include <stdio.h>
int main ()
{
	char ch, c;
	int num;
	printf (" enter character: ");
	scanf ("%c" , &ch);
	for (c = 'a'; ch >= c; c++)
	printf("%c", c);
	for (c = ch - 1; c >= 'a'; c--)
	printf ("%c" , c);
	printf ("\n");
	return 0;
}

