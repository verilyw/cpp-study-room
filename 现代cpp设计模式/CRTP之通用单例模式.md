# CRTP实现通用安全单例

CRTP（Curiously Recurring Template Pattern）主要是为了实现静态多态性。提供在编译期解析函数调用方法

一个模板例子

```c++
template <class Derived>
class Base {
public:
    void interface() {
        // 调用派生类的具体实现
        static_cast<Derived *>(this)->implementation();
    }
};

class Derived: public Base<Derived> {
public:
    void implementation() {
        // 具体实现
    }
}


/// 具体的例子
//// 假设你正在开发一个消息传递系统，其中有多个消息类型需要处理。你可以创建一个基类模板，定义一个通用的消息处理接口，并派生出不同的消息类作为模板参数。每个消息类都可以提供自己的具体实现，用于处理特定类型的消息

#include <iostream>

// Base template class for messages
template <typename Derived>
class Message {
public:
    void Process() {
        // Call the implementation provided by the derived class
        static_cast<Derived*>(this)->ProcessImpl();
    }
};

// Derived message classes
class GreetingMessage : public Message<GreetingMessage> {
public:
    void ProcessImpl() {
        std::cout << "Hello, world!" << std::endl;
    }
};

class WarningMessage : public Message<WarningMessage> {
public:
    void ProcessImpl() {
        std::cout << "Warning: Something went wrong!" << std::endl;
    }
};

int main() {
    GreetingMessage greeting;
    WarningMessage warning;

    greeting.Process(); // Output: "Hello, world!"
    warning.Process();  // Output: "Warning: Something went wrong!"

    return 0;
}
```


## 单例并非线程安全

[C++ and the Perils of Double-Checked Locking](https://blog.csdn.net/m0_37809890/article/details/103880873)

## 通用安全单例

```c++
template <class T>
struct Singleton {
    Singleton() = default;
    virtual ~Singleton() = default;

    Singleton(const Singleton&) = delete;
    void operator=(const Singleton&) = delete;
    Singleton(const Singleton&&) = delete;
    void operator=(const Singleton&&) = delete;

    static T& get() {
        static T instance{};
        return instance;
    }
};

struct A : Singleton<A> {
    A() : id(42), name("Tom") {}

    int id;
    std::string name;
};

struct B: Singleton<B> {
    std::vector<int> v;

    B() {
        v = {1, 2, 3, 4, 5};
    }
};

int main() {
    auto &a = A::get();
    auto &b = B::get();
    
    [&]() {
        std::cout << a.id << " " << a.name << std::endl;
    }();
    return 0;
}
```