/*
// Tomás Oliveira e Silva, AED, October 2021
**
** my first C++ program (compare with hello.c)
*/

#include <iostream>

int main(void)
{
  int x;
  std::cout << "Hello world!\n"; // "same" as printf("Hello world\n");
  //  return 0;  // main() returns 0 if no return value is given
  std::cin>>x;
  x=x+10;
  std::cout<<x;
}
