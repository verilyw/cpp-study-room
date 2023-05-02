# C++移动语义

## 基础特性

> This chapter demonstrates the basic principles and benefits of move semantics.

Note the following two very important things to understand about move semantics:

### 强大的移动语义

move语义减少了c++很多非必要的拷贝操作，特别是在函数传参和函数返回（NRVO）的时候。

+ `std::move(s)` only **marks** to be moable in this context. It does not move anything. It only says, "**I no longer need this value here**". It allows the implementation of the call to benefit from this mark by performing some optimization when copying the value, such as stealing the memory. Whether the value is moved is something the caller does not know.

+ However, an optimization that steals the value has to ensure that the source object is still in a valid state. A moved-from object is neither partially nor fully destroyed. The C++ standard library formulates this for its types as follows: after an operation called for an object marked with `std::move()`, the object is in a **valied but unspecified state.**

#### 拷贝和移动

#### Copying as a Fallback

We saw that by using temporary objects or marking objects with `std::move()` we can enable move semantuics. Functions providing special implementations (by taking non-const rvalue reference) can optimize the copying of a value by "stealing" the value from the source. However, **if there is no optimized version of a function for move semantics, then the usual copying is used as a fallback.**

#### Move semantics for `const` Objects

```c++
template <typename T>
class vector {
public:
/// ...

// insert a copy of elem
void push_back(const T &elem);

// insert elem when the value of elem is no longer need
void push_back(T &&elem);

/// ...
}
```

Note that objects declared with `const` cannot be moved because any optimizing implementation requires that the passed argument can be modified. We cannot steal a value steal a value if we are not allowd to modify it.

```c++
std::vector<std::string> coll;
const std::string s{"data"};

coll.push_back(std::move(s));  // OK, calls push_back(const std::string &)
```

**In princinple, we could provide a special overlaod for this case by declaring a function taking a `const rvalue reference`. However, this makes no semantics sence. Again, the `const lvalue reference` overload serves as a fallback to handle this case.**

##### `const` Return Values

**The fact `const` disable move semantics also has consequences for declaring return types. A `const` return value cannot be moved.**

#### 总结

1. 移动语义可以优化对象的复制，当我们不再需要该值时可以使用它。
2. std::move() 表示在此处不再需要该值，并将对象标记为可移动的。
3. 通过声明具有非凡值引用的函数，调用者在语义上声明不再需要传递的值，函数实现者可以使用此信息来优化其任务。
4. C ++标准库的移动对象仍然是有效的对象，但您不再知道其值。
5. 拷贝语义用作移动语义的回退，如果没有采用对rvalue引用的实现，则使用采用普通const的lvalue引用的任何实现。
6. 即使明确标记了对象为std::move()，也会使用此回退。
7. 对const对象调用std::move()没有任何效果。
8. 如果按值（而不是按引用）返回，请勿将返回值作为整体声明为const，因为这会禁止移动语义，可能会导致不必要的复制。

### 移动语义的核心特性

#### 右值引用

move semantics allows us to optimize using a value of a source that no longer needs thevalue. If compilers automatically detect that a value is used from an object that is at the end of its lifetime,they will automatically switch to move semantics.

`std::move` is nothing but a `static_cast` to an rvalue reference.
```c++
foo(static_cast<decltype<obj&&>>(obj));  // same effect as foo(std::move(obj))
```
Note that the `static_cast` does a bit more than only changing the type of the object here. It also enables the object to be passed to an rvalue reference (**remember that passing objects with names to rvalue referencesis usually not allowed**).

#### 总结
1. Rvalue引用使用&&声明，没有const。
2. 它们可以通过没有名称的临时对象或标记为std::move()的non-const对象进行初始化。
3. Rvalue引用扩展了按值返回的对象的生命周期。
4. std::move()是对相应的rvalue引用类型进行的static_cast。这使我们可以将命名对象传递给rvalue引用。
5. 对象标记为std::move()也可以传递给以const lvalue引用接受参数但不接受non-const lvalue引用的函数。
6. 对象标记为std::move()也可以传递给按值接受参数的函数。在这种情况下，移动语义用于初始化参数，这可以使调用按值传递非常廉价。
7. const rvalue引用是可能的，但针对它们进行实现通常没有意义。
8. 被移动的对象应该是有效但未指定状态。C ++标准库保证其类型。您仍然可以（重新）使用它们，只要不对它们的值做任何假设。


### Move Semantices in Classes

当以下特殊的成员函数被声明时，移动构造的自动生成会被禁用：

+ Copy constructor
+ Copy assignment operator
+ Another move operation
+ Destructor

**记住，任何显示声明了拷贝构造，拷贝复制或者析构函数都将关闭移动语义的自动生成**

#### Implementing Special Copy/Move Memeber functions

```c++
class Customer {
private:
    std::string name;   // name of the customer
    std::vector<int> values;    // some values of the customer
public:
    Customer(const std::string& n): name{n} {
        assert(!name.empty());
    }
    // Copy constructor (copy all members)
    Customer(const Customer& cust): name{cust.name}, values{cust.values} {
        std::cout << "COPY " << cust.name <<'\n';
    }
    // Move constructor (move all members)
    Customer(Customer&& cust) noexcept : name{std::move(cust.name)}, values{std::move(cust.values)} {
        std::cout << "MOVE " << name <<'\n';
    }
    // Copy assignment (assign all members)
    Customer& operator= (const Customer& cust) {
        std::cout << "COPYASSIGN " << cust.name <<'\n';
        name = cust.name;
        values = cust.values;
        return *this;
    }
    // Move assignment (move all members)
    Customer& operator= (Customer&& cust) {
        std::cout << "MOVEASSIGN " << cust.name <<'\n';
        name = std::move(cust.name);
        values = std::move(cust.values);
        return *this;
    }
    
    std::string getName() const {
        return name;
    }
    void addValue(int val) {
        values.push_back(val);
    }
    friend std::ostream& operator<< (std::ostream& strm, const Customer& cust) {
        strm << '[' << cust.name << ": ";
        for (int val : cust.values) {
            strm << val << ',';
        }
        strm << ']';
        return strm;
    }
};
```

注意到在移动构造的时候，我们需要使用`std::move`将参数再次标记成移动语义，这里有个重要的知识点： **move semantics is not passed through**。在使用原对象cust的成员来初始化当前对象的对应成员时，必须使用 std::move() 来标记它们。否则，我们将仅仅复制它们，而当前对象的移动构造函数的性能将与复制构造函数相同。因此，使用 std::move() 的目的是将左值转为右值，实现移动构造或移动赋值，而不是简单的复制操作。

至于为什么移动语义不能够传递，一个简单的例子很容易说明问题。
```c++
void insertTwice(std::vector<std::string> &coll, std::string &&str) {
  coll.push_back(str);  // copy str into coll
  coll.push_back(std::move(str)); // move str into coll
}
```

**Declared Copying Disables Moving (Fallback Enabled). Declared Moving Disables Copying**

从上述表述中，我们可以知道，当把移动构造声明为`deleted`，那么既不能移动也不能拷贝（自动关闭拷贝操作）

#### 总结

1. 移动语义不会自动传递。

2. 每个类都会自动生成移动构造函数和移动赋值运算符，除非不可能。

3. 用户定义复制构造函数、复制赋值运算符或析构函数会禁用类中的自动移动语义支持，但不会影响派生类的支持。

4. 用户定义移动构造函数或移动赋值运算符会禁用自动复制语义支持，在这种情况下将得到仅支持移动的类型。

5. 永远不要删除特殊的移动成员函数。

6. 如果没有特殊需求，尽可能避免声明析构函数，特别是在从多态基类派生的类中。

### How benefit from move semantics

不要在`setter`中使用"take by value and move"（不包括容器里面添加元素），这可能会产生事与愿违的事情。因此，**将参数按值传递并将其移动到需要新值的位置仅在我们将传递的值存储在某个地方作为新值（因此我们无论如何都需要新内存）时才有用。当修改现有值时，这种策略可能会产生反效果。**

#### 在继承中使用移动语义

如果继承需要声明`virtual`析构函数，这会关闭移动语义的自动生成，而当我们显示声明移动语义时，则会关闭拷贝构造的生成。因此，如果要使用他们，必须显示声明。

#### 总结
1. 避免给对象命名。
2. 避免不必要的 std::move()，特别是在返回局部对象时。
3. 构造函数从具有便宜的移动操作的参数初始化成员时，应该通过值取参数，并将其移动到成员。
4. 构造函数从需要较长移动操作的参数初始化成员时，应该重载移动语义以获得最佳性能。
5. 从具有便宜的移动操作的参数创建和初始化新值时，应该按值和移动获取参数，但不要按值和移动来修改现有值。
6. 除非必要，否则不要在派生类中声明虚析构函数。

### Overloading on reference qualifiers

> this chapter will answer the common question about whether getters should return by value or by constant reference.


考虑一个使用`const ref`返回不安全的例子：
```c++
class Person {
private:
    std::string name;
///....
public:
    auto getName() const -> const std::string& {
        return name;
    }
};

for (char c : Person().getName()) { // Undefined behavios
    if (c == ' ') {
        // ...
    }
}

reference range = Person().getName();
// OPPS: returned temporary object destryed here.
for (auto pos = range.begin(), end = range.end(); pos != end; ++pos) { //...}
```

使用移动语义解决
```c++
class Person {
/// ....

auto getName() && -> std::string { // when we no longer need the value, we steal and return by value
    return std::move(name);
}

auto getName() const& -> const std::string& { // in another case, we give access to the member
    return name;
}
};
```
+ The version with the &&qualifier is used when we have an object where we no longer need the value (anobject that is about to die or that we have marked withstd::move()).

+ The version with the const& qualifier is used in all other cases.  It always fits but is only the fallback ifwe cannot take the &&version. Thus, this function is used if we have an object that is not about to die ormarked withstd::move()

#### 总结

1. 可以根据不同的引用限定符（reference qualifiers）对成员函数进行重载。
2. 为昂贵的成员重载getter函数并加上引用限定符，使其安全又快速。
3. 在调用成员函数时，即使接受对象已经是右值引用，加上std::move()也有意义。
4. 在赋值运算符中使用引用限定符。

## 泛型中的移动语义

## STL中的移动语义

