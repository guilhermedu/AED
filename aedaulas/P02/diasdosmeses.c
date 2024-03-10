#include<stdlib.h>
#include<stdio.h>
#include<errno.h>

//escrever um programa que tenha um array com os dias de cada mes e que depois os some escrevendoos no terminal
/*int main(int argc,char *argv[argc]){
    int dias[12]={31,28,31,30,31,30,31,31,30,31,30,31};
    //ESCREVER ARRAY DIAS
    for(int i=0;i<12;i++){
        printf("dias[%d]=%d\n",i,dias[i]);
    }
    int soma=0;
    for(int i=0;i<12;i++){
        soma+=dias[i];
    }
    printf("A soma dos dias dos meses é %d\n",soma);
    return 0;
}*/
void PrintArray(char *s, int a[], int n){
    printf("%s\n", s);
    for(int i=0; i<n; i++){
        printf("%d ", a[i]);
    }
    printf("\n");
}   

void ComulativeSum(int a[], int b[], int n){
    int c = 0;
    for(int i=0; i<n; i++){
        c += a[i];
        b[i] = c;
    }
}

int main(void){
    int dias[12]={31,28,31,30,31,30,31,31,30,31,30,31};
    size_t size =12;
    PrintArray("dias",dias,size);
    int b[size];

    ComulativeSum(dias,b,size);
    PrintArray("b",b,size);
    return 0;
}

   