#include <cstdio>
#include <type_traits>
#include "print.h"

template <typename T0, typename ...Ts>
auto func(T0 t0, Ts ...ts) {
  // (print(ts), ...);
  printnl("{");
  printnl(t0);
  ([&]{
    printnl(", ");
    printnl(ts);
  }(), ...);
  print("}\n");
}

// class <typename 

int main() {
  // auto x = (1, 2, 4);
  // print(x);
  // func();
  // func(1);
  // func(1, 2);
  // func(1, 2, 3);
  // func(1, 2, 3, 4);
  return 0;
}