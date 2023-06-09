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
  void WLock() {
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

  /**
   * Release a write latch.
   */
  void WUnlock() {
    std::lock_guard<mutex_t> guard(mutex_);
    writer_entered_ = false;
    reader_.notify_all();
  }

  /**
   * Acquire a read latch.
   */
  void RLock() {
    std::unique_lock<mutex_t> latch(mutex_);
    while (writer_entered_ || reader_count_ == MAX_READERS) {
      reader_.wait(latch);
    }
    reader_count_++;
  }

  /**
   * Release a read latch.
   */
  void RUnlock() {
    std::lock_guard<mutex_t> guard(mutex_);
    reader_count_--;
    if (writer_entered_) {
      if (reader_count_ == 0) {
        writer_.notify_one();
      }
    } else {
      if (reader_count_ == MAX_READERS - 1) {
        reader_.notify_one();
      }
    }
  }

 private:
  mutex_t mutex_;
  cond_t writer_;
  cond_t reader_;
  uint32_t reader_count_{0};
  bool writer_entered_{false};
};

}  // namespace bustub