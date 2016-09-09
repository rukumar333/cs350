#include <stdio.h>

void swap(int *a, int *b);

int main(){
    int a = -1;
    int b = 10;
    printf("Before --> a: %d, b: %d\n", a, b);
    swap(&a, &b);
    printf("After  --> a: %d, b: %d\n", a, b);
    return 0;
}

void swap(int *a, int *b){
    *b = *a + *b;
    *a = *b - *a;
    *b = *b - *a;
}
