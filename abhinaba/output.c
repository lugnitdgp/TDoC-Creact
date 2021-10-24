#include<stdio.h>

float fact(float x);
void main(){

int var1, res=0;
printf("Enter a value: ");
scanf("%d",&var1);
res = fact(var1);
printf("%d\n",(res));
}
float fact(float x){
float f=1,i;
for(i=1;i<=x;i++){
f=f*i;
}
return f;
}
