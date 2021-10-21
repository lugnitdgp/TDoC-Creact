#include<stdio.h>

void main(){

int b=1;
if(b%2 == 1){
if(b>2){
printf("%d",b);
}
else{
printf("hello");
}
}
else if(b==3){
 b = b*4 ;
printf("%d",b);
}
else{
 b = b*2 ;
printf("%d",b);
}
}
