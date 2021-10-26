#include<stdio.h>

#include<stdlib.h>
#define predefsz 2
int c167= 0;
void main(){

int v212 = predefsz;
int *v = (int)malloc(sizeof(int)*v212);
int x,i,j,tmp,a,b;
printf("Enter number of elements to enter: \n");
scanf("%d",&x);
for(i=0;i<x;i++){
int y;
scanf("%d",&y);
v=checkout(c167,&v212,v);
*(v+c167++)=y;
}
printf("The original Array is :\n");
show(v,c167);
for(i=0;i<x-1;i++){
for(j=0;j<x-1;i++){
a = *(v+j);
b = *(v+j+1);
if(a>b){
tmp = *(v+j);
*(v+j) = *(v+j+1);
*(v+j+1) = tmp;
}
}
}
printf("The sorted array is : \n");
show(v,c167);
}
