# pragma once

#include <cmath>
#include <random>
#include <vector>
#include <utility>
#include <mutex>

namespace kvstore {

template <typename K, typename V>
class SkipNode {

public:
    explicit SkipNode(K key, V value, bool is_sentinel=false)
        : key_(key), value_(value), is_sentinel_(is_sentinel) {}

    auto Key() const -> K {
        return key_;
    }

    auto Value() const -> V {
        return value_;
    }
    
    void SetValue(V value) {
        value_ = value;
    }

    auto IsSentinel() -> bool {
        return is_sentinel_;
    }

    auto Before() -> SkipNode * & {
        return before_;
    }

    auto After() -> SkipNode * & {
        return after_;
    }

    auto Above() -> SkipNode * & {
        return above_;
    }

    auto Below() -> SkipNode * & {
        return below_;
    }

    auto SkipSearch(K key) -> std::pair<SkipNode *, std::vector<SkipNode *>> {
        auto curr = this;
        std::vector<SkipNode *> path;
        SkipNode * prev = nullptr;

        do {
            while(curr->ShouldSkipRight(key)) {
                curr = curr->after_;
            }
            path.push_back(curr);
            prev = curr;
            curr = curr->below_;
        } while(curr != nullptr);

        return {prev, path};
    }

private:

    auto ShouldSkipRight(K key) -> bool {
        if (after_ == nullptr || after_->IsSentinel()) {
            return false;
        }
        return after_->Key() <= key;
    }

private:
    bool is_sentinel_;
    K key_;
    V value_;

    SkipNode * before_ {nullptr};
    SkipNode * after_ {nullptr};
    SkipNode * above_ {nullptr};
    SkipNode * below_ {nullptr};
};

template <typename K, typename V>
class SkipList {
public:
    explicit SkipList(int max_height=10) : max_height_(max_height) {
        //! create first layer of sentinel nodes
        head = new SkipNode<int, int>(K{}, V{}, true);
        auto tail = new SkipNode<int, int>(K{}, V{}, true);
        head->After() = tail;
        tail->Before() = head;
        curr_height_ = 1;
    }
    ~SkipList() {
        //! release every level
        while(head != nullptr) {
            auto temp = head->Below();
            ReleaseLevel(head);
            head = temp;
        }
    }

    auto MaxHeight() const -> int {
        return max_height_;
    }

    void SetMaxHeight(int h) {
        max_height_ = h;
    } 

    auto Height() const -> int {
        return curr_height_;
    }

    auto Size() const -> int {
        return curr_size_;
    }

    auto Search(K key) -> SkipNode<K, V> * {
        return head->SkipSearch(key).first;
    }

    auto Insert(K key, V value) -> bool {
        std::lock_guard<std::mutex> lk{mutex_};
        auto search_pair = head->SkipSearch(key);
        auto match = search_pair.first;
        auto path = search_pair.second;

        if (match->Key() == key && !match->IsSentinel()) {
            /// 1. key already exists, the whole column's value shold be update.
            auto curr = match;
            while(curr != nullptr) {
                curr->SetValue(value);
                curr = curr->Above();
            }
            return false;
        }

        /// 2. key doesn't exists, a new key-value to be inserted.
        int extend_height = rand() % max_height_ + 1;
        if (curr_height_ < extend_height) {
            // build enough extra layer
            int diff = extend_height - curr_height_;
            while(diff--) {
                BuildExtraLayer();
            }
            // refetch the search path(not optimal)
            path = head->SkipSearch(key).second;
        }
        // build the new node all the way up, after each path[i]
        SkipNode<K, V> *last = nullptr;
        for (int i = path.size() - 1; i >= 0; --i) {
            auto new_node = new SkipNode<K, V>(key, value, false);
            auto before = path[i];
            auto after = path[i]->After();
            before->After() = new_node;
            new_node->Before() = before;
            new_node->After() = after;
            after->Before() = new_node;
            if (last) {
                last->Above() = new_node;
                new_node->Below() = last;
            }
            last = new_node;
        }
        curr_size_ += 1;
        // dynamic update max height
        max_height_ = std::max(max_height_, ExpectHeight());
        return true;
    }

    auto Remove(K key) -> bool {
        std::lock_guard<std::mutex> lk{mutex_};
        auto search_pair = head->SkipSearch(key);
        auto match = search_pair.first;
        
        if (match->Key() == key && !match->IsSentinel()) {
            /// key exists, remove the tower of the key
            auto curr = match;
            while (curr != nullptr) {
                auto temp = curr;
                auto before = curr->Before();
                auto after = curr->After();
                curr = curr->Above();
                before->After() = after;
                after->Before() = before;
                delete temp;
            }
            curr_size_ -= 1;
            return true;
        }
        // key doesn't exsit.
        return false;
    }

private:

    auto ExpectHeight() -> int {
        return static_cast<int>(log2(curr_size_) + 2);
    }

    void BuildExtraLayer() {
        auto new_head = new SkipNode<K, V>(K{}, V{}, true);
        auto new_tail = new SkipNode<K, V>(K{}, V{}, true);
        head->Above() = new_head;
        new_head->Below() = head;
        new_head->After() = new_tail;
        new_tail->Before() = new_head;

        // find the old top-level tail
        auto curr = head;
        while(curr->After() != nullptr) {
            curr = curr->After();
        }
        new_tail->Below() = curr;
        curr->Above() = new_tail;
        curr_height_ += 1;
        head = new_head;
    }

    void ReleaseLevel(SkipNode<K, V> * node) {
        while(node != nullptr) {
            auto t = node->After();
            delete node;
            node = t;
        }
    }
    
private:
    int max_height_;
    int curr_height_ {0};
    int curr_size_ {0};
    SkipNode<K, V> * head {nullptr};
    
    std::mutex mutex_;
};

}
