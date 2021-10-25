#include<stdio.h>

#include <stdlib.h>
#define predefsz 2
#define predefsz1 4
#include <stdlib.h>
#define predefsz 2
#define predefsz1 4
int c121= 0;
int c236= 0;
int* checkout(int counter, int *pre, int *ptr){
    if(counter==*(pre)){
        ptr =  (int*)realloc(ptr, sizeof(int)*(*pre)*2);
        (*pre) = (*pre)*2;
    }
    return ptr;
}
void show(int *ptr, int counter){
    for(int i=0; i<=counter; i++){
        printf("%d\t", *(ptr+i));
    }
    printf("\n");
}

float* checkout1(int counter, int *pre, float *ptr){
    if(counter==*(pre)){
        ptr =  (float*)realloc(ptr, sizeof(float)*(*pre)*2);
        (*pre) = (*pre)*2;
    }
    return ptr;
}
void show1(float *ptr, int counter){
    for(int i=0; i<=counter; i++){
        printf("%f\t", *(ptr+i));
    }
    printf("\n");
}
void main(){

float v204 = predefsz1;
float *v = (float*)malloc(sizeof(float)*v204);
float vector2857 = predefsz1;
float *vector2 = (float*)malloc(sizeof(float)*vector2857);
int x,i,j, a,b;
float tmp;
printf("Enter the number of elements to enter: \n");
scanf("%d",&x);
for(i=0;i<x; i++){
float y;
scanf("%f",&y);
*(v+c121++)=y;
}
c121--;
printf("The original Array is: \n");
for(i=0; i<x-1; i++){
for(j=0;j<x-i-1;j++){
a = *(v+j);
b = *(v+j+1);
if(a>b){
tmp = *(v+j);
*(v+j) = *(v+j+1);
*(v+j+1) = tmp;
}
}
}
printf("The sorted Array is \n");
}
