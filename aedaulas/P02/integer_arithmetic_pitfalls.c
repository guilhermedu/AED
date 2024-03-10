//
// Tomás Oliveira e Silva, AED, October 2021
//
// explain the program output
//
// try also compiling the program with the -Wsign-compare compilation flag
//

#include <stdio.h>

int main(void)
{
  unsigned int i = 1;// valor de variaveis de tipos diferentes !!!!
  int j = -1;
  int k = -2147483648;

  printf("original i = %u\n",i);
  printf("original j = %d\n",j);
  printf("original k = %d\n",k);
  // compare i with j
  if(i > j)
    printf("i > j is true\n");
  else
    printf("i > j is false\n");
  // replace k by its absolute value and print the result
  if(k < 0)
    k = -k;//overflow número demasiado elevevado  limits.h
  printf("absolute value of k = %d\n",k);
  return 0;
}
