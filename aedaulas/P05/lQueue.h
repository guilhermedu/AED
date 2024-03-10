//
// Tomás Oliveira e Silva, AED, November 2021
//
// Generic queue (First In First Out) implementation based on an linked list
//

#ifndef _AED_lQueue_
#define _AED_lQueue_

#include <cassert>
#include "sList.h"

template <typename T>
class lQueue
{
  private:sList<T> list;
  public:
    lQueue(void)
    {
      

    }
    ~lQueue(void)
    {
    }
    void clear(void)
    {
      list.size==0; // fazer um ciclo while e retirar o 1 elemento sucessivamente;
      return list;
    }
    int size(void) const
    {
      
    }
    int is_full(void) const
    {
      return 0;
    }
    int is_empty(void) const
    {
      if (list.size() == 0)
        return 1;
      else
        return 0;
    }
    void enqueue(T &v)
    {
    }
    T dequeue(void)
    {
    }
    T peek(void)
    {
    }
};

#endif
