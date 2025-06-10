// main.cpp
#include <iostream>

/**
 *
 */
int main()
{
#ifdef _DEBUG
  std::cout << "Hello, mbgclient debug hihi!" << std::endl;
#else
  std::cout << "Hello, mbgclient release hehe!" << std::endl;
#endif

  return 0;
}