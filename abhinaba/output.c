#include<stdio.h>

void main(){

int var1,var2,var3;
scanf("%d %d %d",&var1,&var2,&var3);
if(var1<var2){
if(var1<var3){
printf("Minimum is %d",var1);
}
else{
printf("Minimum is %d",var3);
}
}
else{
if(var2<var3){
printf("Minimum is %d",var2);
}
else{
printf("Minimum is %d",var3);
}
}
}
