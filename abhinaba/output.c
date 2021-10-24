#include<stdio.h>

void main(){

int var1, i=0;
printf("Enter a value: ");
scanf("%d",&var1);
printf("Printing for loop: ");
for(i=0; i<var1; i++){
printf("%d ",i);
}
printf("Printing while loop: ");
while(i<var1){
printf("%d",i);
i++;
}
}
