#include <iostream>
#include <array>

template <class t, size_t n>
struct array {
  t* begin() { return arr;}
  t* end() { return arr + n; }
  t arr[n];
};

// 类模板推导指引
template<typename tp, typename... up>
  array(tp, up...)
    -> array<std::enable_if_t<(std::is_same_v<tp, up> && ...), tp>,
       1 + sizeof...(up)>;

int main()
{
  ::array arr{1, 2, 3, 4, 5};
  for (const auto &i: arr) {
    std::cout << i << " ";
  }
  return 0;
}