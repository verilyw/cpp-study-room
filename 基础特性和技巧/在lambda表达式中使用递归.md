
经典斐波那契案例

```c++
int main() 
{
	auto fib = [&](auto &&fib, int n) -> int {
		if (n <= 1) return n;
		return fib(fib, n - 1) + fib(fib, n - 2);
	};
	std::cout << fib(fib, 8) << std::endl;
	return 0;
}
```
将名字作为参数传递。