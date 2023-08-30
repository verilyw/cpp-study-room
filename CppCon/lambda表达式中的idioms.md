# C++ Lambda Expression


## Unary plus trick

```c++
int main() {
    auto *ptr = [](int i) { return i * i; } // ERROR! can not deduce type for `auto*`

    auto *ptr = +[](int i) { return i * i; } // OK! static cast lambda to a pointer
}
```

## 立即求值 (IIFE, immediately invoke function expression)
## Call-by-once

```c++
int main()
{
    std::vector<Foo> foos;
    foo.emplace_back([&]{
        if (hasDatabase) {
            return getFooFromDatabase();
        } else {
            return getFooFromElsewhere();
        }
    }());
}   
```

## Call-once

```c++
struct X {
    X() {
        static auto _ = []{ std::cout << "call once !\n"; return 0; }();
    }
};

int main() {
    X x1;
    X x2;
    X x3;
}
```

## Variable template lambda

```c++
template <typename T>
auto c_cast = [](auto x) -> T {
    return (T)x;
};

int main() {
    int x = c_cast<int>(3.1415);
    std::cout << x << std::endl;
}
```

## Lambda overload set (c++20)

```c++
template <typename ...Ts>
struct overload: Ts... {
    using Ts::operator()...;
};

int main() {

    overload f = {
        [](int i) { std::cout << "int thingy\n"; },
        [](float i) { std::cout << "float thingy\n"; }
    };

    std::variant<int, float> v = 2.0f;
    std::visit(f, v);
}
```
