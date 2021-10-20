#include<stdio.h>
int main() {
int  var1, var2;
char  var3 ;
float  var4 ;
printf("Enter a character:");
scanf("%c",&var3);
printf("Enter two numbers:");
scanf("%d%d",&var1, &var2);
printf("Enter a float:");
scanf("%f",&var4);
printf("The values entered are: \n var1 = %d \n var2 = %d \n var3 = %c \n var4 = %f \n", var1, var2, var3, var4);
}
