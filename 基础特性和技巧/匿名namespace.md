# 匿名namespace

当定义一个命名空间时，可以忽略这个命名空间的名称：
```c++
namespace {
  int a;
  void foo() {}
}
```

编译器在内部会为这个命名空间生成一个唯一的名字，而且还会为这个匿名的命名空间生成一条using指令.

```c++
namespace __unique_name__{
  int a;
  void foo() {}
}
using namespace __unique_name__;
```

在匿名命名空间中声明的名称也将被编译器转换，与编译器为这个匿名命名空间生成的唯一内部名称(即这里的_UNIQUE_NAME)绑定在一起

值得注意的是（ODR原则）：
**这些名称具有internal链接属性，这和声明为static的全局名称的链接属性是相同的，即名称的作用域被限制在当前文件中，无法通过在另外的文件中使用extern声明来进行链接**。如果不提倡使用全局static声明一个名称拥有internal链接属性，则匿名命名空间可以作为一种更好的达到相同效果的方法。

**命名空间都是具有external 连接属性的,只是匿名的命名空间产生的UNIQUE_NAME在别的文件中无法得到,这个唯一的名字是不可见的.**


其和全局static作用类似，都可以防止符号重定义（ORD原则）。区别在于：

+ static需要对每个变量或者函数加上关键字。而匿名namespace要方便一些。
+ static不能作用于类或结构体 。而匿名namespace可以。

