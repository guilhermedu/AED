#include<errno.h>
#include<stdio.h>
#include<stdlib.h>


int Integer(char *string){
    int possible_integer=isdigit(string[0]);
}
int main(int argc,char *argv[argc])
{
   for(int i=0;i<argc;i++){
    char *token = argv[i];
    if(IsInteger(token)){
        printf("argv[%2d] = \"%s\" is an integer with value %d\n",i,token,ato1(token));
    }
    else{
        printf("argv[%2d] = \"%s\" is not an integer\n",i,token);
    }
       
   }
  
}