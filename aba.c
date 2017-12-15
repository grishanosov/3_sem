#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void GenerateString(int n, char* string);

int main()
{
	int n;
	scanf("%d", &n);
	char* string = malloc(sizeof(char) * (1 << n));
	GenerateString(n, string);	
	free(string);
	return 0;
}

void GenerateString(int n, char* string)
{
	*string = 'a';
	int len, i;
	for (i = 1; i < n; i++)
	{
		len = strlen(string);
		string[len] = 'a' + i;
		strncpy(string + len + 1, string, (1 << i) - 1);
		len = strlen(string);
		string[len] = '\0';
	}
	printf("%s\n", string);
}
