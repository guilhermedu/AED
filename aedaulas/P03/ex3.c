#include <stdio.h>

void DisplayPol(double* coef, unsigned int degree){
    printf("P(x) = ");
    for(int i=0; i<degree; i++){
        printf("%f*x^%d", coef[i], degree-i-1);
        if(i != degree-1){
            printf(" + ");
        }
    }
    printf("\n");
}

int main( void){
    double coef[3] = {1,4,1};
    DisplayPol(coef, 3);
    double coef2[8] = {1,2,3,4,5,6,7,8};
    DisplayPol(coef2, 8);
    return 0;
}