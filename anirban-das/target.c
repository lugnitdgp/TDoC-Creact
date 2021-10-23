#include<stdio.h>

double product(int var1,double var2);
void main(){

int var1;
double var2, res=0;
printf("Enter Value1: ");
scanf("%d",&var1);
printf("Enter Value2: ");
scanf("%lf",&var2);
res = product(var1, var2);
printf("Product: %lf \n",res);
}
double product(int var1,double var2){
double pdt=1;
 pdt = var1*var2 ;
return pdt;
}
