#include<stdio.h>

#include <stdlib.h>
#define predefsz 2
int c327= 0;
int *checkout1(int counter,int *pre,int *ptr){
    if(counter==(*pre)){
        ptr=(int*)realloc(ptr,sizeof(int)*(*pre)*2);
        (*pre)=(*pre)*2;
    }
    return ptr;
}

float *checkout2(int counter,int *pre,float *ptr){
    if(counter==(*pre)){
        ptr=(float*)realloc(ptr,sizeof(float)*(*pre)*2);
        (*pre)=(*pre)*2;
    }
    return ptr;
}

void show1 (int *ptr,int counter){
    for(int i=0;i<counter;i++){
        printf("%d\t",*(ptr+i));
    }
    printf("\n");
}

void show2 (float *ptr,int counter){
    for(int i=0;i<counter;i++){
        printf("%f\t",*(ptr+i));
    }
    printf("\n");
}
void main(){

int v120 = predefsz;
float *v = (float*)malloc(sizeof(float)*v120);
int x,i;
float a;
printf("Enter the number of elements to enter\n");
scanf("%d",&x);
for(i=0;i<x;i++){
float y;
scanf("%f",&y);
v=checkout2(c327,&v120,v);*(v+c327++)=y;
}
printf("The original Array is:\n");
show2(v,c327);
a = *(v+0);
printf("The First element is: %f",(a));
}
