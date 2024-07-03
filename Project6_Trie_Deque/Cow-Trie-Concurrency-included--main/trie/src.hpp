#ifndef SJTU_TRIE_HPP
#define SJTU_TRIE_HPP

#include <algorithm>
#include <cstddef>
#include <map>
#include <memory>
#include <optional>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>
#include <string_view>
#include <iostream>
#include <shared_mutex>
#include <string_view>
#include <mutex>
#include <future>

namespace sjtu
{

  // A TrieNode is a node in a Trie.
  class TrieNode
  {
  public:
    // Create a TrieNode with no children.
    TrieNode() = default;

    // Create a TrieNode with some children.
    explicit TrieNode(std::map<char, std::shared_ptr<const TrieNode>> children) : children_(std::move(children)) {}

    virtual ~TrieNode() = default;

    // Clone returns a copy of this TrieNode. If the TrieNode has a value, the value is copied. The return
    // type of this function is a unique_ptr to a TrieNode.
    //
    // You cannot use the copy constructor to clone the node because it doesn't know whether a `TrieNode`
    // contains a value or not.
    //
    // Note: if you want to convert `unique_ptr` into `shared_ptr`, you can use `std::shared_ptr<T>(std::move(ptr))`.
    virtual auto Clone() const -> std::unique_ptr<TrieNode> { return std::make_unique<TrieNode>(children_); }

    // A map of children, where the key is the next character in the key, and the value is the next TrieNode.
    std::map<char, std::shared_ptr<const TrieNode>> children_;

    // Indicates if the node is the terminal node.
    bool is_value_node_{false};

    // You can add additional fields and methods here. But in general, you don't need to add extra fields to
    // complete this project.
  };

  // A TrieNodeWithValue is a TrieNode that also has a value of type T associated with it.
  template <class T>
  class TrieNodeWithValue : public TrieNode
  {
  public:
    // Create a trie node with no children and a value.
    explicit TrieNodeWithValue(std::shared_ptr<T> value) : value_(std::move(value)) { this->is_value_node_ = true; }

    // Create a trie node with children and a value.
    TrieNodeWithValue(std::map<char, std::shared_ptr<const TrieNode>> children, std::shared_ptr<T> value)
        : TrieNode(std::move(children)), value_(std::move(value))
    {
      this->is_value_node_ = true;
    }

    // Override the Clone method to also clone the value.
    //
    // Note: if you want to convert `unique_ptr` into `shared_ptr`, you can use `std::shared_ptr<T>(std::move(ptr))`.
    auto Clone() const -> std::unique_ptr<TrieNode> override
    {
      return std::make_unique<TrieNodeWithValue<T>>(children_, value_);
    }

    // The value associated with this trie node.
    std::shared_ptr<T> value_;
  };

  // A Trie is a data structure that maps strings to values of type T. All operations on a Trie should not
  // modify the trie itself. It should reuse the existing nodes as much as possible, and create new nodes to
  // represent the new trie.
  class Trie
  {
  private:
    // The root of the trie.
    std::shared_ptr<const TrieNode> root_{nullptr};

    // Create a new trie with the given root.
    explicit Trie(std::shared_ptr<const TrieNode> root) : root_(std::move(root)) {}

  public:
    // Create an empty trie.
    Trie() = default;

    // Get the value associated with the given key.
    // 1. If the key is not in the trie, return nullptr.
    // 2. If the key is in the trie but the type is mismatched, return nullptr.
    // 3. Otherwise, return the value.
    template <class T>
    auto Get(std::string_view key) const -> const T *
    {
      if (root_ == nullptr)
        return nullptr;
      auto cur = root_;
      for (auto ch : key)
      {
        if (cur->children_.find(ch) == cur->children_.end())
          return nullptr;
        cur = cur->children_.at(ch);
      }
      if (cur->is_value_node_)
      {
        auto valuenode = dynamic_cast<const TrieNodeWithValue<T> *>(cur.get());
        return valuenode == nullptr ? nullptr : valuenode->value_.get();
      }
      return nullptr;
    }

    // Put a new key-value pair into the trie. If the key already exists, overwrite the value.
    // Returns the new trie.
    template <class T>
    auto Put(std::string_view key, T value) const -> Trie
    {
      Trie new_trie;
      auto value_ptr = std::make_shared<T>(std::move(value));
      auto cur = root_ ? root_->Clone() : std::make_shared<TrieNode>();
      std::shared_ptr<TrieNode> par;
      new_trie.root_ = cur;
      for (auto ch : key)
      {
        par = cur;
        auto new_node = (cur->children_.find(ch) != cur->children_.end()) ? std::shared_ptr<TrieNode>(cur->children_[ch]->Clone()) : std::make_shared<TrieNode>();
        cur->children_[ch] = new_node;
        cur = new_node;
      }
      if (!key.empty())
      {
        auto ch = key.back();
        par->children_[ch] = std::make_shared<TrieNodeWithValue<T>>(std::move(cur->children_), std::move(value_ptr));
      }
      else
        new_trie.root_ = std::make_shared<TrieNodeWithValue<T>>(std::move(cur->children_), std::move(value_ptr));
      return new_trie;
    }

    // Remove the key from the trie. If the key does not exist, return the original trie.
    // Otherwise, returns the new trie.
    auto Remove(std::string_view key) const -> Trie
    {
      Trie new_trie;
      auto cur = root_;
      new_trie.root_ = cur;
      std::vector<std::shared_ptr<TrieNode>> path;
      for (auto ch : key)
      {
        if (cur->children_.find(ch) == cur->children_.end())
          return new_trie;
        path.push_back(cur->Clone());
        cur = cur->children_.at(ch);
      }
      path.push_back(cur->Clone());
      for (int i = 0; i < path.size() - 1; i++)
        path[i]->children_[key[i]] = path[i + 1];
      new_trie.root_ = path[0];
      if (!path.back()->children_.empty()) // end_node can't be deleted
      {
        auto new_node = std::make_shared<TrieNode>(std::move((path.back()->children_)));
        path[path.size() - 2]->children_[key.back()] = new_node;
      }
      else
      {
        for (int i = key.size() - 1; i >= 0; i--)
        {
          path[i]->children_.erase(key[i]);
          if ((!path[i]->children_.empty()) || path[i]->is_value_node_) // if the node in the middle has value or other sons delete is over
            break;
        }
      }
      return new_trie;
    }
  };

  // This class is used to guard the value returned by the trie. It holds a reference to the root so
  // that the reference to the value will not be invalidated.
  template <class T>
  class ValueGuard
  {
  public:
    ValueGuard(Trie root, const T &value) : root_(std::move(root)), value_(value) {}
    auto operator*() const -> const T & { return value_; }

  private:
    Trie root_;
    const T &value_;
  };

  // This class is a thread-safe wrapper around the Trie class. It provides a simple interface for
  // accessing the trie. It should allow concurrent reads and a single write operation at the same
  // time.
  class TrieStore
  {
  public:
    // This function returns a ValueGuard object that holds a reference to the value in the trie. If
    // the key does not exist in the trie, it will return std::nullopt.
    template <class T>
    auto Get(std::string_view key) -> std::optional<ValueGuard<T>>
    {
      auto snap_trie = Trie();
      {
        std::lock_guard<std::mutex> lock(root_lock_);
        snap_trie = root_;
      }
      auto value = snap_trie.Get<T>(key);
      if (value)
        return ValueGuard(snap_trie, *value);
      else
        return std::nullopt;
    }

    // This function will insert the key-value pair into the trie. If the key already exists in the
    // trie, it will overwrite the value.
    template <class T>
    void Put(std::string_view key, T value)
    {
      auto snap_trie = Trie();
      {
        std::lock_guard<std::mutex> lock(root_lock_);
        snap_trie = root_;
      }
      std::lock_guard<std::mutex> lock(write_lock_);
      root_ = snap_trie.Put<T>(key, std::move(value));
    }

    // This function will remove the key-value pair from the trie.
    void Remove(std::string_view key)
    {
      auto snap_trie = Trie();
      {
        std::lock_guard<std::mutex> lock(root_lock_);
        snap_trie = root_;
      }
      std::lock_guard<std::mutex> lock(write_lock_);
      root_ = snap_trie.Remove(key);
    }

  private:
    // This mutex protects the root. Everytime you want to access the trie root or modify it, you
    // will need to take this lock.
    std::mutex root_lock_;

    // This mutex sequences all writes operations and allows only one write operation at a time.
    std::mutex write_lock_;

    // Stores the current root for the trie.
    Trie root_;
  };

} // namespace sjtu

#endif // SJTU_TRIE_HPP