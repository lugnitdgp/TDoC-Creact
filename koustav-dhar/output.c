#include <stdio.h>

int gcd_algo ( int a , int b ) ;
void main(){

int val,val1,res;
printf("Enter the 2 values to get gcd \n");
scanf("%d%d",&val,&val1);
if(val>val1){
 res=gcd_algo(val1,val);
printf("%d",res);
} 
else if(val<val1){
 res=gcd_algo(val,val1);
printf("%d\n",res);
} 
else{
printf("%d\n",val);
} 
}
int gcd_algo ( int a , int b ) {
if(a==0){
return  b;
} 
else{
return  gcd_algo(b%a,a);
} 
}
