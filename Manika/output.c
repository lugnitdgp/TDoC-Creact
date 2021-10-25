#include<stdio.h>

void main(){

int var1,var2,var3;
printf("Enter 3 values ");

scanf("%d %d %d",&var1,&var2,&var3);
if(var1>var2){
if(var1>var3){
printf("Maximum is %d",var1);

}
else{
printf("Maximum is %d",var3);

}
}
else{
if(var2>var3){
printf("Maximum is %d",var2);

}
else{
printf("Maximum is %d",var3);

}
}
}
