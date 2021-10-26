#include<stdio.h>

#include <stdlib.h>
#include <string.h>
#define BuffSize 2
void main(){

char *str = malloc(BuffSize);
char a;
printf("Enter string\n");
int ch;int size = BuffSize;int i = 0;
while ((ch = fgetc(stdin)) != '\n')
{ str[i++] = ch;
if (i == size + BuffSize)
{ size += BuffSize; str = realloc(str, size); }
}
str[i] = 0;
a = *(str+0);
printf("The First element is: %c\n",(a));
printf("The string is: %s",(str));
}
