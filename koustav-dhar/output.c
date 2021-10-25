#include <stdio.h>

#include "modules/dynamicarray.h"
#define PI 3.14
void main(){

printf("Hello, World\n");
printf("- From Koustav\n");
float pi=PI;
printf("pi = %f\n",pi);
int c0 = 0;
int v0 = SIZE;
float *v = (float*)malloc(sizeof(float)*v0);
int x,i,j;
float tmp;
printf("Enter the number of elements to enter: ");
scanf("%d",&x);
for(i=0;i<x;++i){
float y;
scanf("%f",&y);
v=checkoutfloat(c0,&v0,v);
*(v+c0++)=y;
} 
printf("The original array is: \n");
showfloat(v,c0);
for(i=0;i<x-1;++i){
for(j=0;j<x-i-1;++j){
if(v[j] > v[j+1]){
tmp=v[j];
v[j]=v[j+1];
v[j+1]=tmp;
} 
} 
} 
printf("The sorted array is: \n");
showfloat(v,c0);
}
