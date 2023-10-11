# X-macro技术

+ 这种方式能够很好地应用在枚举类型反射上，同时减少代码量，不用再去写大量的`if-else`或者`case-by-case`的语句。


```c++
#include <string>
#include <optional>
#define FOREACH_COLOR(f) \
  f(red) \
  f(blue ) \
  f(green) \
  f(orange) \
  f(violet) \
  f(white) \
  f(black)  

enum class Color {
#define _FUNCTION(name) name,
  FOREACH_COLOR(_FUNCTION)
#undef _FUNCTION
};


auto get_color_from_name(const std::string &color) -> std::optional<Color> {
#define _FUNCTION(name) if(color == #name) return Color::name; 
  FOREACH_COLOR(_FUNCTION)
#undef _FUNCTION

  return std::nullopt;
} 

auto get_color_name(Color color) -> std::optional<std::string> {
  switch (color) {
#define _FUNCTION(name) case Color::name: return #name;
    FOREACH_COLOR(_FUNCTION)
#undef _FUNCTION
  }

  return std::nullopt;
}
```