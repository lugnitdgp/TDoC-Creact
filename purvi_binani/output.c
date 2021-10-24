#include<stdio.h>

#include <stdlib.h>
#define predefsz 2
int c254= 0;
int *checkout(int counter,int *pre,int *ptr){
    if(counter==(*pre)){
        ptr=(int*)realloc(ptr,sizeof(int)*(*pre)*2);
        (*pre)=(*pre)*2;
    }
    return ptr;
}
void show (int *ptr,int counter){
    for(int i=0;i<counter;i++){
        printf("%d\t",*(ptr+i));
    }
    printf("\n");
}
void main(){

int v207 = predefsz;
int *v = (int*)malloc(sizeof(int)*v207);
int x,i,j,tmp,a,b;
printf("Enter the number of elements to enter");
scanf("%d",&x);
for(i=0;i<x;i++){
int y;
scanf("%d",&y);
v=checkout(c254,&v207,v);*(v+c254++)=y;
}
printf("The original Array is:");
show(v,c254);
for(i=0;i<x-1;i++){
for(j=0;i<x-i-1;j++){
a = *(v+j);
b = *(v+j+1);
if(a>b){
tmp = *(v+j);
*(v+j) = *(v+j+1);
*(v+j+1) = tmp;
}
}
}
printf("The sorted Array is:");
show(v,c254);
}
