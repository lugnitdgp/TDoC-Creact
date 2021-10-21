#include <stdio.h>

void main(){

printf("Hello, World\n");
printf("- From Koustav\n");
int num1=10,num2;
scanf("%d",&num2);
num1+=num2%7;
printf("%d\n",num1);
if(num1<5){
printf("Inside If\n");
}
else if((num1>7) && (num1<20)){
if(num1<15){
printf("Inside Nested If\n");
}
}
}
