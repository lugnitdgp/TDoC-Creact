#include <stdlib.h>
#define SIZE 2

int* checkoutint(int counter, int *pre, int *ptr) {
    if (counter == *pre) {
        ptr = (int*)realloc(ptr, sizeof(int)*(*pre)*2);
        *pre = (*pre) * 2;
    }
    return ptr;
}
void showint(int *ptr, int counter) {
    for (int i = 0; i < counter; ++i) {
        printf("%d ", ptr[i]);
    }
    printf("\n");
}

char* checkoutchar(int counter, int *pre, char *ptr) {
    if (counter == *pre) {
        ptr = (char*)realloc(ptr, sizeof(char)*(*pre)*2);
        *pre = (*pre) * 2;
    }
    return ptr;
}
void showchar(char *ptr, int counter) {
    for (int i = 0; i < counter; ++i) {
        printf("%c ", ptr[i]);
    }
    printf("\n");
}

float* checkoutfloat(int counter, int *pre, float *ptr) {
    if (counter == *pre) {
        ptr = (float*)realloc(ptr, sizeof(float)*(*pre)*2);
        *pre = (*pre) * 2;
    }
    return ptr;
}
void showfloat(float *ptr, int counter) {
    for (int i = 0; i < counter; ++i) {
        printf("%f ", ptr[i]);
    }
    printf("\n");
}