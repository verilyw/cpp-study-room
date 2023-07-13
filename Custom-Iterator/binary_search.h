#pragma once

#include <cstddef>
#include <initializer_list>
#include <iterator>
#include <memory>

struct BstNode {
  std::unique_ptr<BstNode> left {nullptr};
  std::unique_ptr<BstNode> right {nullptr};

  int value;
  
  explicit BstNode(int value) : value(value) {}
};


// Bst Iterator 实现

class BstIterator {
public:
  // Iterator traits, previously from std::iterator
  using value_tyep = int;
  using difference_type = std::ptrdiff_t;
  using pointer = int *;
  using reference = int &;
  using iterator_category = std::bidirectional_iterator_tag;

  BstIterator() = default;
  explicit BstIterator(BstNode *node);

  reference operator*() const;

  BstIterator& operator++();  // pre
  BstIterator operator++(int);  // post

  BstIterator& operator--();
  BstIterator operator--(int);

  bool operator==(const BstIterator &rhs);
  bool operator!=(const BstIterator &rhs);

private:
  
};


class BinarySearchTree {
public:
  using iterator = BstIterator;
  
  BinarySearchTree() = default;
  explicit BinarySearchTree(std::initializer_list<int> il) {
    for (auto n : il) {
      Insert(n);
    }
  }

  BinarySearchTree(const BinarySearchTree &other) = delete;
  void operator = (const BinarySearchTree &other) = delete;

  void Insert(int value) {
    insert(root, value);
  } 

  iterator begin() const;
  iterator end() const;

private:
  std::unique_ptr<BstNode> root {nullptr};

  void insert(std::unique_ptr<BstNode> &node, int value) {
    if (node == nullptr) {
      auto new_node = std::make_unique<BstNode>(value);
      node = std::move(new_node);
      return;
    }

    if (value < node->value) {
      insert(node->left, value);
    } else {
      insert(node->right, value);
    }
  }

};

