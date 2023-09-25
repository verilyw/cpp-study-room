#include "ticktock.h"
#include <iostream>
#include <vector>
#include <memory_resource>
#include <list>

int main()
{
  std::pmr::unsynchronized_pool_resource pool;
  std::pmr::monotonic_buffer_resource mem;
  std::vector<int, std::pmr::polymorphic_allocator<char>> v{&mem};
  TICK(vector)
  for (int i = 0; i < 65535; ++i) {
    v.push_back(i);
  }
  TOCK(vector);


  std::list<int> l;
  TICK(list)
  for(int i = 0; i < 65535; ++i) {
    l.push_back(i);
  }
  TOCK(list)
  return 0;

}
