#include<stdio.h>
#include <stdlib.h>
#define predefsz 2
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
int c0= 0;int c1= 0;
int main(){
int at0 = predefsz;int *at = (int*)malloc(sizeof(int)*at0);int ptr1 = predefsz;int *ptr = (int*)malloc(sizeof(int)*ptr1);ptr=checkout(c1,&ptr1,ptr);
*(ptr+c1++)=1;
ptr=checkout(c1,&ptr1,ptr);
*(ptr+c1++)=2;
ptr=checkout(c1,&ptr1,ptr);
*(ptr+c1++)=3;
ptr=checkout(c1,&ptr1,ptr);
*(ptr+c1++)=4;
show(ptr,c1);
 ptr[2]=8 ;
show(ptr,c1);
int a,b,c;
printf("No of numbers you want to take : \n");
scanf(" %d",&a);
for(b=0;b < a;b++){
scanf(" %d",&c);
at=checkout(c0,&at0,at);
*(at+c0++)=c;
}
show(at,c0);
printf("Deleting one number\n");
c0--;
show(at,c0);
}