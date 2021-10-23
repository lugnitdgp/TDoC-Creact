#include <stdio.h>

int fact ( int num ) ;
void voila ( char x ) ;
void main(){

printf("Hello, World\n");
printf("- From Koustav\n");
int num1=10,num2=5,num3;
printf("\nEnter number for factorial:");
scanf("%d",&num3);
 voila('y');
 num2=fact(num3);
printf("${ans}\n");
}
int fact ( int num ) {
int i=1,ans=1;
while (i<num) {
ans*=i;
i++;
}
return  ans;
}
void voila ( char x ) {
printf("Hello %c!\n",x);
}
