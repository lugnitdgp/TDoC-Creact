#include<stdio.h>
#include<stdlib.h>

int *checkout(int counter, int *pre, int *ptr){
    if(counter == (*pre)){
        ptr = (int*)realloc(ptr, sizeof(int)*(2*(*pre)));
        (*pre) = (2*(*pre));
    }
    return ptr;
}
void show(int *ptr, int counter){
    for(int i=0; i<counter; i++){
        printf("%d\t", *(ptr+i));
    }
    printf("\n");
}