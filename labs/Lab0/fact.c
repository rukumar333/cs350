#include <stdio.h>

int factorial(int num);

int main(){
    int fact = factorial(5);
    printf("%d\n", fact);
    return 0;
}

int factorial(int num){
    if(num < 0){
	printf("Invalid input");
	return -1;
    }
    if(num == 0){
	return 1;
    }else{
	return num * factorial(num - 1);
    }
}
