#include "revert_string.h"
#include <string.h>

void Swap(char *left, char *right)
{
    char temp;

    temp = *left;
    *left = *right;
    *right = temp;
}


void RevertString(char *str)
{
	unsigned int length = strlen(str);
	
	for(int i=0; i < length/2; i++)
	{
		Swap(&str[i],&str[length - 1 - i]);
	}
}
