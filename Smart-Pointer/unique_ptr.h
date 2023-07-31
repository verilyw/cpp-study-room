#pragma once
#include <iostream>
#include <memory>

namespace wt {


// 实现一个简单的unique_ptr
// 
template <typename T>
class UniquePtr {
public:
    UniquePtr(T &t);
    
    UniquePtr(const UniquePtr &&rhs) noexcept
        : ptr_(rhs.release()) {}

    UniquePtr& operator = (const UniquePtr &&rhs) noexcept {
        ptr_ = rhs.release();
        return *this;
    }

    // 操作符重载
    T* operator -> () const noexcept {
        return get();
    }
    
    T& operator *() const noexcept { // 解引用
        return *get();
    }  

    operator bool() const noexcept {
        return get();
    }

    // 获取原始指针
    T*  get() const noexcept {
        return ptr_;
    }

    // 释放所有权并返回指针
    T* release() noexcept {
        T *temp = ptr_;
        ptr_ = nullptr;
        return temp;
    }

    void swap(UniquePtr &&rhs) noexcept {
        using std::swap;
        swap(rhs.ptr_, this->ptr_);
    }

private:
    T *ptr_;
};



}