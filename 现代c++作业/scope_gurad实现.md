要求实现scope_guard类型（即支持传入任意可调用类型，析构的时候同时调用）

为什么需要scope_guard？

+ 对于某些资源（c风格）无法用c++的RAII，帮助他们能够像c++RAII那样管理资源。

```c++
struct scope_gurad {
    // 使得c风格文件接口能够如其他c++标准库一般
    // 在离开作用域后时总是能够释放资源。
}

int main()
{
    FILE *fp;
    try {
        fp = std::fopen("test.txt", "w+");
        auto guard = scope_guard([&] {
            std::fclose(fp);
            fp = nullptr;
        });

        throw std::logic_error("test");
    } catch (...) {
        printf("%d", fp == nullptr);
    }
}
```

解答：

```c++
#include <cstdio>
#include <iostream>
#include <functional>

struct X{
	X() { puts("X()"); }
	X(const X&) { puts("X(const X&)"); }
	X(X&&)noexcept { puts("X(X&&)"); }
	~X() { puts("~X()"); }
};

template <class F, class ...Args>
    requires requires(F f, Args ...args) { std::invoke(f, args...); }
struct scope_guard {
    F f;
    std::tuple<Args...> values;
    
    template <typename Fn, typename ...Ts>
    scope_guard(Fn&& func, Ts&&... args)
        : f{std::forward<Fn>(func)}, values{std::forward<Ts>(args)...} {}
    ~scope_guard() {
        std::apply(f, values);
    }

    scope_guard(const scope_guard&) = delete;
    void operator=(const scope_guard&) = delete;
};

// 推导指引
template <class F, class ...Args>
scope_guard(F&&, Args&&...) -> scope_guard<std::decay_t<F>, std::decay_t<Args>...>;


int main(){
	{
		auto x = new X{};
		auto guard = scope_guard([&] {
			delete x;
			x = nullptr;
		});
	}
	puts("----------");
	{
		struct Test {
			void operator()(X*& x) {
				delete x;
				x = nullptr;
			}
		};
		auto x = new X{};
		Test t;
		auto guard = scope_guard(t, x);
	}
	puts("----------");
	{
		struct Test{
			void f(X*&x){
				delete x;
				x = nullptr;
			}
		};
		auto x = new X{};
		Test t;
		auto guard = scope_guard{ &Test::f,&t,x };//error
	}
}
```