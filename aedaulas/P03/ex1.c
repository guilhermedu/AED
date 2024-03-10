#include <stdio.h>


void f1 (int a, int b){ // call-by-value, o que entra na funcao é o valor de a e b
    a = 10;
    b = 100;
}

void f2 (int *a, int *b){ // call-by-value, o que entra na funcao é o endereco de a e b
    *a = 1000;
    *b = 10000;
}
void swap1 (int a, int b){                                                                              
    int temp;
    a=b;
    b=temp;
}
void swap2(int *a, int *b){
    int temp ;
    temp = *a;
    *a = *b;
    *b = temp;
}

int main(void){
    int x = 0;
    int y = 1;

    f1(x,y);
    printf("%d %d\n", x, y);
    f2(&x,&y);          // passar o endereco de x e y
    printf("%d %d\n", x, y);
    swap1(x, y);
    printf("%d %d\n", x, y);
    swap2(&x, &y);
    printf("%d %d\n", x, y);
    return 0;
}