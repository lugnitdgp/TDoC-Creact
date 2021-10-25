#include<stdio.h>

int fact(int x);
void main(){

int var1,res=0;
printf("Enter a value \n");
scanf("%d",&var1);
res= fact(var1);
printf("%d\n",res);
}
int fact(int x){
int f=1,i;
for(i=1;i<=x;i++){
f=f*i;
}
return f;
}
