//
// Tomás Oliveira e Silva, AED, October 2021
//
// list all command line arguments
//

#include <stdio.h>

int main(int argc,char *argv[argc])
{
  for(int i = 0;i < argc;i++)//argc= numero de palavras argumento count  //arg vector array de caracteres 
    printf("argv[%2d] = \"%s\"\n",i,argv[i]);
  return 0;
}
