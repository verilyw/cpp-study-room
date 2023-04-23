# 如何写一个读写锁

当我们想要并发时，在多线程下，一个正确的，高性能的锁是必不可少的。本节将会分析一个读写锁。

## 要求

+ 数据结构有读者和写者，简而言之，就是一个数据结构包含读写

+ 读者从数据结构中读取，而写者会修改已有的数据结构

+ 一次只能有一个写者对数据结构进行修改

+ 当有一个活跃的写者在写入时，任何读者不应该能够访问数据结构

+ 当没有写者处于活跃状态，多个读者可以同时从数据结构中读取。


一些版本的读写锁会考虑读优先还是写优先。

## 语言

现代c++拥有支持并发编程的标准库，如互斥量，条件变量，线程库等。

1. `std::mutex`是同步原语。不可复制，不可移动。用户可以在共享的`std::mutex`调用lock和unlock功能。以便只有一个线程获取锁，进入临界区。

2. `std::lock_guard`是`std::mutex`的RAII风格包装。一旦创建，它将尝试获取互斥量的所有权，当超出作用域时，将会自动释放互斥量。

3. `std::unique_lock`是一个通用的`std::mutext`包装器，与上面`std::lock_guard`相似，但更灵活。它可以在其生命周期的中间解锁互斥量，并和条件变量一起使用。

4. `std::conditon_variable`是同步原语。可以让多个线程“等待”。并且对该条件变量的操作能够通知等待线程的notify_one或者notify_all。

5. **注意：**在条件变量上调用`wait`时将自动释放它持有的互斥锁。当它被notify_one或者notify_all唤醒时，将再次获得互斥锁。

## 代码

```c++
//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// rwmutex.h
//
// Identification: src/include/common/rwlatch.h
//
// Copyright (c) 2015-2019, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#pragma once

#include <climits>
#include <condition_variable>
#include <mutex>

namespace bustub {

/**
 * Reader-Writer latch backed by std::mutex.
 */
class ReaderWriterLatch {
  using mutex_t = std::mutex;
  using cond_t = std::condition_variable;
  static const uint32_t MAX_READERS = UINT_MAX;

 public:
  ReaderWriterLatch() = default;
  ~ReaderWriterLatch() { std::lock_guard<mutex_t> guard(mutex_); }
  ReaderWriterLatch(const ReaderWriterLatch &) = delete;
  ReaderWriterLatch &operator=(const ReaderWriterLatch&) = delete;

  /**
   * Acquire a write latch.
   */
  void WLock();

  /**
   * Release a write latch.
   */
  void WUnlock();

  /**
   * Acquire a read latch.
   */
  void RLock();

  /**
   * Release a read latch.
   */
  void RUnlock();

 private:
  mutex_t mutex_;
  cond_t writer_;
  cond_t reader_;
  uint32_t reader_count_{0};
  bool writer_entered_{false};
};

}  // namespace bustub
```

+ 写锁

```c++
void ReaderWriterLatch::WLock() {
    std::unique_lock<mutex_t> latch(mutex_);
    while (writer_entered_) {
        /* Only one writer could be writing at the same time */
        reader_.wait(latch);
    }
    /* this thread is the one who are going to write next */
    writer_entered_ = true;
    /* after setting true, no more reader allowed to pass */
    while (reader_count_ > 0) {
        /* let any remaining reader finish their reading first */
        writer_.wait(latch);
    }
}
```

线程可能在两个地方等待读者：

1. WLock()因为只有一个写着可以继续，另一个写者必须等待。
2. RLock()因为当有活跃的写者时不能读。

```c++
void ReaderWriterLatch::RLock() {
    std::unique_lock<mutex_t> latch(mutex_);
    while (writer_entered_ || reader_count_ == MAX_READERS) {
        reader_.wait(latch);
    }
    reader_count_++;
}
```


```c++
void ReaderWriterLatch::WUnlock() {
    std::lock_guard<mutex_t> guard(mutex_);
    writer_entered_ = false;
    reader_.notify_all();  // 这里唤醒的可能有等待读的，也可能有等待获取写的
}
```


```c++
void ReaderWriterLatch::RUnlock() {
    std::lock_guard<mutex_t> guard(mutex_);
    reader_count_--;
    if (writer_entered_) {
        if (reader_count_ == 0) {
        writer_.notify_one();   // 当所有读者读完成读了，唤醒写者
        }
    } else {
        if (reader_count_ == MAX_READERS - 1) {
        reader_.notify_one();
        }
    }
}
```