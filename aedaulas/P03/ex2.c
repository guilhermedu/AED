#include <stdio.h>

void Display(int* a, size_t size){ 
    printf ("Array = [") ;
    for(int i=0; i<size; i++){
        printf("%d", a[i]);
    }
    printf("]\n");
}

int* Read(size_t *size){
    assert(size != NULL)

    size_t n;
    printf("Array size (>0): ");
    scanf("%u", &n);
    assert(n>0);

    int* array = malloc(n * sizeof(int));
    if(array == NULL){
        *size = 0;
        return NULL;
    }

    for (size_t i=0; i<n; i++){
        printf("array[%2u] = ", i);
        scanf("%d", &array[i]);
    }

    *size = n;
    return array;
}

void Apend(int* a, int* b, size_t size_a, size_t size_b){
    assert(a != NULL);
    assert(b != NULL);
    assert(size_a > 0);
    assert(size_b > 0);

    int* c = malloc((size_a + size_b) * sizeof(int));
    if(c == NULL){
        return;
    }

    for(size_t i=0; i<size_a; i++){
        c[i] = a[i];
    }

    for(size_t i=0; i<size_b; i++){
        c[size_a + i] = b[i];
    }

    free(a);
    free(b);

    a = c;
    b = NULL;
    size_a += size_b;
    size_b = 0;
}



int main (void){
    int a[5] = {1,2,3,4,5};
    size_t size = 5;
    Display(a, size);
    
    size_t size2 = 4;
    int a2[4] = {1,2,3,4};
    Display(a2, size2);

    int* newArray = Apend(a, a2, size, size2);
    Display(newArray, size);
    return 0;
}