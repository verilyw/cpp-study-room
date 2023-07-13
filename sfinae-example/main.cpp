#include <iostream>
#include <cstdio>
#include <type_traits>
#include <string>
#include <vector>
#include "print.h"

template <typename T>
auto func(T const &t) {
  if constexpr (std::is_same_v<std::decay_t<decltype(t)>, int>) {
    return t + 1;
  } else {
    return t.substr(1);
  }
}

// template <class F>
// auto invoke(F f) {
//   print("entern\n");
//   // std::is_void_v<std::invoke_result_t<F>>
//   // std::is_void_v<decltype(std::declval<F>()())
//   if constexpr (std::is_void_v<decltype(f())>) {
//     f();
//     print("leaved\n");
//   } else {
//     auto ret = f();
//     print("leaved\n");
//     return ret;
//   }
// }

#define REQUIRES(X) std::enable_if_t<(X), int> = 0 // SFINAE

template <typename F, REQUIRES(std::is_void_v<std::invoke_result_t<F>>)>
auto invoke(F f) {
  printf("entered void\n");
  f();
  printf("leaved void\n");
}

template <typename F, REQUIRES(!std::is_void_v<std::invoke_result_t<F>>)>
auto invoke(F f) {
  printf("enterd non-void\n");
  auto ret = f();
  printf("leaved non-void\n");
  return ret;
}


// 现在有4个类
struct mystudent {
  void dismantle() {
    printf("rm -rf stu.db\n");
  }

  void rebel(int i) {
    printf("rm -rf gench-%d\n", i);
  }
};

struct myclass {
  void dismantle() {
    printf("rm -rf course\n");
  }
};

struct myteacher {
  void rebel(int i ) {
    printf("rm -rf gench-%d\n", i);
  }
};

struct myvoid {
};

template <typename T, typename = void>
struct has_dismantle{
  static constexpr bool value = false;
};

// 偏特化
template <typename T>
struct has_dismantle<T, std::void_t<decltype(std::declval<T>().dismantle())>> {
  static constexpr bool value = true;
};

template <typename T, typename = void>
struct has_rebel {
  static constexpr bool value = false;
};

template <typename T>
struct has_rebel<T, std::void_t<decltype(std::declval<T>().rebel(std::declval<int>()))>> {  // SFINAE
  static constexpr bool value = true;
};

// cpp17
template <typename T>
void gench(T t) {
  if constexpr(has_dismantle<T>::value) {
    t.dismantle();
  } else if constexpr (has_rebel<T>::value) {
    t.rebel(2);
  } else {
    printf("no any method supported\n");
  }
}


// template <typename T, REQUIRES()

int main()
{
  myclass mc;
  mystudent ms;
  myteacher mt;
  myvoid mv;
  gench(mc);
  gench(mt);
  gench(ms);
  gench(mv);
  return 0;
}
