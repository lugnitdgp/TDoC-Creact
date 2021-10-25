#include<stdio.h>

#include <stdlib.h>
#define predefsz 2
#define predefsz1 4
#include <stdlib.h>
#define predefsz 2
#define predefsz1 4
int c181= 0;
int c476= 0;
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
void main(){

int v337 = predefsz;
int *v = (int*)malloc(sizeof(int)*v337);
int vector2175 = predefsz;
int *vector2 = (int*)malloc(sizeof(int)*vector2175);
int x,i,j, a,b;
int tmp;
printf("Enter the number of elements to enter: \n");
scanf("%d",&x);
for(i=0;i<x; i++){
int y;
scanf("%d",&y);
v=checkout(c181,&v337,v);*(v+c181++)=y;
}
c181--;
printf("The original Array is: \n");
show(v,c181);
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
show(v,c181);
}
