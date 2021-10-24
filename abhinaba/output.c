#include<stdio.h>

void main(){

int var1, i=0;
printf("Enter a value: ");
scanf("%d",&var1);
printf("Printing first %d values of 2's table\n",(var1));
for(i=1; i<=var1; i++){
printf("%d\n",(i*2));
}
}
