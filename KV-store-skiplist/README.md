# 基于SkipList设计一个Key-Value Store



key-value存储在现实世界中被广泛使用，是许多应用程序不可或缺的一部分，它提供插入，删除，以及快速访问操作。

+ 抽象数据结构，SkipList
+ SkipList的C++实现
+ 性能测试和优化

## SkipList

(Reference: <[Data Structures & Algorithm in C++](#)>)

A interesting data structure for efficiently realizing the ordered map ADT is the **skip list**.

跳表通过随机选择条目(entries)排列实现了有序映射的功能。搜索和更新在O(logn)时间。

SkipList中的条目按照h个级别的方式排序。底层的`h0`包含所有的有序方式的条目。上面的每个级别都将包含其下面级别中的条目子集，也是以有序的方式。

![avatar](https://github.com/YukunJ/Teach-Myself-CPP/raw/main/KV-store-skiplist/Fig/skiplist_overview.png)

SkipList支持三个API：`search`, `insert`, `delete`
(SkipList的起始位置在左上角的位置，$key=-\infty$，n表示SkipList中的条目数)

+ Search

```c++
Algorithm SkipSearch(k):
    Time complexity: O(logn)
    Input: A search key k
    Output: Position p in the bottom list S0 such that the entry at p has the largest key less than or equal to k

    p <- s
    while (below(p) != null) do
        p <- below   // go down one level
        while ( k >= key(after(p))) do 
            p <- after(p)
    return p
```
上述算法从顶层开始，尽可能向右走，然后向下一层继续向右走，最后，将以`key <= k`的位置结束，如果`key=k`，说明我们找到了key。

+ Insert

```c++
Algorithm SkipInsert(k, v):
    Time Complexity: O(logn)
    Input: Key k and value v
    Output: Topmost position of the entry inserted in the skip list
	
	p <- SkipSeach(k)
	q <- null
    e <- (k, v)
	i <- -1
	repeat
        i <- i + 1
        if i >= h then  // add a new level to the skip list
            h <- h + 1
            t <- after(s)
            s <- InsertAfterAbove(null, s, (-oo, null))
            InsertAfterAbove(s, t, (+oo, null))
		while (above(p) == null) do
            p <- before(p)	// sacn backward
        p <- above(p)		// jump up to higher level
        q <- InsertAfterAbove(p, q, e)	// add a position to the tower of the new entry
	until coinFlip() == tails
	n <- n+1
	return q
```

将上述算法进行分解。搜先利用之前的`SkipSearch`在底部`h0`级别找到小于k的最大key的位置，然后将在这个位置插入新的键值对`(k, v)`，向上移动。

有一个`InsertAfterAbove(after, above, (k, v))`的函数可以帮助插入。在这里使用随机化技术，向上复制新的`(k, v)`，对到某个级别`i`取决于抛硬币。如果我们超过skip list当前高度，将构建一个新的层(级别)。该层上只有两个位置（占位符）：`(-oo, null)`和`(+oo, null)`.

比如插入42：

![avatar](https://github.com/YukunJ/Teach-Myself-CPP/raw/main/KV-store-skiplist/Fig/skiplist_insertion.png)


+ Remove

```c++
Algorithm SkipRemove(k):
    Time complexity: O(logn)
    Input: key k
    Output: True if k deletion is successful, False otherwise
	
    p <- SkipSearch(k)
    if (p == null) then
        return False
	while (p != null) do
        tmp <- above(p)
        connect before(p) and after(p)	// shrink the link
        delete p
        p <- temp
	return True
```

上述删除算法很简单，如果存在k的话，只是删除包含`k`的整个垂直的tower。