#include<stdio.h>

struct student {
int x;
float y;
};
void main(){

struct student a={1,2.5};
struct student b=a;
printf("%d\n",(a.x));
printf("%f\n",(a.y));
printf("%d\n",(b.x));
printf("%f\n",(b.y));
}
