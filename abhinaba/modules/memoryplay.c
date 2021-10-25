#include<stdio.h>
#include<stdlib.h>
#define size 2
int count=0;
int* checkout(int counter, int *pre, int *ptr){
    if(count == (*pre)){
        ptr = (int*)realloc(ptr, sizeof(int)*(*pre)*2);
        (*pre) = (*pre)*2;
    }
    return ptr;
}

void show(int *ptr, int counter){
    for(int i=0; i<counter; i++){
        printf("%d\t", *(ptr+i));
    }
    printf("\n");
}

int main(){
    int sz = size;
    int *ptr = (int*)malloc(sizeof(int)*sz);
    int t=1;
    while(t){
        printf("Enter the value to enter\n");
        int val;
        scanf("%d", &val);
        ptr = checkout(count, &sz, ptr);
        *(ptr+(count++)) = val;
        printf("Do you wish to enter more elements\n");
        scanf("%d", &t);
    }
    show(ptr, count);
    free(ptr);
    return 0;
}