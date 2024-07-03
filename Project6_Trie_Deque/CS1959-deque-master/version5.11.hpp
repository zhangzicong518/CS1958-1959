#ifndef SJTU_DEQUE_HPP
#define SJTU_DEQUE_HPP

#include "exceptions.hpp"

#include <cstddef>
#include <iostream>

namespace sjtu
{

  template <class T>
  class deque
  {
  private:
    size_t capacity;
    double loadingfactor;

  public:
    class vec
    {
    public:
      int size, cap;
      int st;
      T **arr;
      vec(int _cap = 32)
      {
        arr = new T *[_cap];
        cap = _cap;
        size = 0;
        st = 0;
        for (int i = 0; i < cap; i++)
          arr[i] = nullptr;
      }
      vec(const vec &other)
      {
        this->~vec();
        size = other.size;
        cap = other.cap;
        st = 0;
        arr = new T *[cap];
        for (int i = 0; i < cap; i++)
          arr[i] = nullptr;
        for (int i = 0; i < size; i++)
          arr[i] = new T(*other.arr[i + other.st]);
      }
      vec &operator=(const vec &other)
      {
        if (arr == other.arr)
          return *this;
        this->~vec();
        size = other.size;
        cap = other.cap;
        arr = new T *[cap];
        st = 0;
        for (int i = 0; i < cap; i++)
          arr[i] = nullptr;
        for (int i = 0; i < size; i++)
          arr[i] = new T(*other.arr[i + other.st]);
        return *this;
      }
      ~vec()
      {
        for (int i = 0; i < st + size; i++)
          if (arr[i] != nullptr)
            delete arr[i];
        delete[] arr;
      }
      void swap(int x, int y)
      {
        T *tmp = arr[x];
        arr[x] = arr[y];
        arr[y] = tmp;
      }
      void expand()
      {
        cap = cap << 1;
        T **_arr = new T *[cap];
        for (int i = 0; i < cap; i++)
          _arr[i] = nullptr;
        for (int i = 0; i < size; i++)
          _arr[i] = new T(*arr[i + st]);
        for (int i = 0; i < st + size; i++)
          delete arr[i];
        delete[] arr;
        arr = _arr;
        st = 0;
      }
      bool is_empty()
      {
        return size == 0;
      }
      bool is_full()
      {
        return st + size == cap;
      }
      int get_size() const
      {
        return size;
      }
      void push_back(T val)
      {
        if (size == cap)
          expand();
        arr[++size - 1 + st] = new T(val);
      }
      void push_back(T *val)
      {
        if (is_full())
          expand();
        arr[++size - 1 + st] = new T(*val);
      }
      void push_front(T val)
      {
        if (is_full())
          expand();
        size++;
        for (int i = size - 1; i > 0; i--)
          swap(st + i, st + i - 1);
        arr[st] = new T(val);
      }
      void push_front(T *val)
      {
        if (is_full())
          expand();
        size++;
        for (int i = size - 1; i > 0; i--)
          swap(st + i, st + i - 1);
        arr[st] = new T(*val);
      }
      T &operator[](int pos) const
      {
        if (pos < size)
          return *(arr[st + pos]);
        else
          throw("invalid operation");
      }
      T &at(int pos)
      {
        if (pos < size)
          return *(arr[st + pos]);
        else
          throw("invalid operation");
      }
      T &front()
      {
        if (size == 0)
          throw("invalid operation");
        else
          return *(arr[st]);
      }
      T &back()
      {
        if (size == 0)
          throw("invalid operation");
        else
          return *(arr[st + size - 1]);
      }
      void insert(T val, int pos) // insert before
      {
        if (is_full())
          expand();
        size++;
        for (int i = size - 1; i != pos; i--)
          arr[st + i] = arr[st + i - 1];
        arr[st + pos] = new T(val);
      }
      void remove(int pos)
      {
        if (pos < size)
        {
          delete arr[st + pos];
          arr[st + pos] = nullptr;
          for (int i = pos; i <= size - 2; i++)
            swap(st + i, st + i + 1);
          size--;
        }
      }
      void pop_front()
      {
        if (size)
        {
          delete arr[st];
          arr[st] = nullptr;
          for (int i = 0; i <= size - 2; i++)
            swap(st + i, st + i + 1);
          size--;
        }
      }
      void pop_back()
      {
        if (size)
        {
          delete arr[--size + st];
          arr[st + size] = nullptr;
        }
      }
      void resize(int num)
      {
        for (int i = 0; i < size; i++)
          arr[i] = nullptr;
        delete[] arr;
        arr = new T *[num];
        cap = num;
        size = 0;
        st = 0;
        for (int i = 0; i < cap; i++)
          arr[i] = nullptr;
      }
    };

    class block
    {
    public:
      vec elements;
      block *nxt, *pre;
      block(block *_nxt = nullptr, block *_prv = nullptr) : nxt(_nxt), pre(_prv) {}
      block(const block &other)
      {
        nxt = other.nxt;
        pre = other.pre;
        elements = other.elements;
      }
      block &operator=(const block &other)
      {
        elements.~vec();
        nxt = other.nxt;
        pre = other.pre;
        elements = other.elements;
        return *this;
      }
      ~block()
      {
        if (nxt != nullptr)
          delete this->nxt;
        pre = nullptr;
        nxt = nullptr;
      }
    };
    block *firstblock, *lastblock;

    class const_iterator;
    class iterator
    {
    public:
      block *current_block;
      int index;
      iterator(block *_current_block = nullptr, int _index = 0) : index(_index), current_block(_current_block) {}
      iterator(const iterator &t)
      {
        index = t.index;
        current_block = t.current_block;
      }
      std::pair<block *, int> GetPos() const
      {
        int num = index + 1;
        block *tmp = current_block->pre;
        while (tmp != nullptr)
        {
          num += tmp->elements.size;
          tmp = tmp->pre;
        }
        tmp = current_block;
        while (tmp->pre != nullptr)
          tmp = tmp->pre;
        return std::make_pair(tmp, num);
      }

      int operator-(const iterator &rhs) const
      {
        std::pair<block *, int> res1, res2;
        res1 = GetPos();
        res2 = rhs.GetPos();
        if (res1.first != res2.first)
          throw("invalid operation");
        return res1.second - res2.second;
      }
      iterator &operator+=(const int &n)
      {
        if (n > 0) // + n
        {
          if (n + index < current_block->elements.size)
            index += n;
          else
          {
            int res = n - (current_block->elements.size - index);
            current_block = current_block->nxt;
            while (current_block != nullptr && current_block->nxt != nullptr && res >= current_block->elements.size)
            {
              res -= current_block->elements.size;
              current_block = current_block->nxt;
            }
            if (current_block->nxt == nullptr && res)
              throw("index_out_of_bound");
            else
              index = res;
          }
        }
        else if (n < 0) // - n
        {
          int res = -n;
          if (index >= res)
            index -= res;
          else
          {
            res = res - index;
            current_block = current_block->pre;
            while (current_block != nullptr && res > current_block->elements.size) // judge nullptr first
            {
              res -= current_block->elements.size;
              current_block = current_block->pre;
            }
            if (current_block == nullptr && res)
              throw("index_out_of_bound");
            else
              index = current_block->elements.get_size() - res;
          }
        }
        return *this;
      }
      iterator &operator-=(const int &n)
      {
        if (n > 0)
        {
          if (index >= n)
            index -= n;
          else
          {
            int res = n - index;
            current_block = current_block->pre;
            while (current_block != nullptr && res > current_block->elements.size) // judge nullptr first
            {
              res -= current_block->elements.size;
              current_block = current_block->pre;
            }
            if (current_block == nullptr && res)
              throw("index_out_of_bound");
            else
              index = current_block->elements.size - res;
          }
        }
        else if (n < 0)
        {
          int res = -n;
          if (res + index < current_block->elements.size)
            index += res;
          else
          {
            res = res - (current_block->elements.size - index);
            current_block = current_block->nxt;
            while (current_block->nxt != nullptr && res >= current_block->elements.size)
            {
              res -= current_block->elements.size;
              current_block = current_block->nxt;
            }
            if (current_block != nullptr && current_block->nxt == nullptr && res)
              throw("index_out_of_bound");
            else
              index = res;
          }
        }
        return *this;
      }
      iterator operator+(const int &n) const
      {
        iterator tmp = *this;
        tmp += n;
        return tmp;
      }
      iterator operator-(const int &n) const
      {
        iterator tmp = *this;
        tmp -= n;
        return tmp;
      }
      iterator &operator++()
      {
        if (current_block->nxt == nullptr) // check end()
          throw("invalid operation");
        if (index + 1 == current_block->elements.get_size())
        {
          index = 0;
          current_block = current_block->nxt;
        }
        else
          index++;
        return *this;
      }

      iterator operator++(int)
      {
        iterator tmp = *this;
        ++(*this);
        return tmp;
      }

      iterator &operator--()
      {
        if (index == 0)
        {
          current_block = current_block->pre;
          if (current_block == nullptr)
            throw("invaid operation");
          index = current_block->elements.get_size() - 1;
        }
        else
          index--;
        return *this;
      }

      iterator operator--(int)
      {
        iterator tmp = *this;
        --(*this);
        return tmp;
      }

      T &operator*() const
      {
        if (current_block->elements.arr[current_block->elements.st + index] == nullptr)
          throw("invalid operation");
        return this->current_block->elements.arr[current_block->elements.st + index];
      }

      T *operator->() const noexcept
      {
        if (current_block->elements.arr[current_block->elements.st + index] == nullptr)
          throw("invalid operation");
        return &(current_block->elements.arr[current_block->elements.st + index]);
      }

      bool operator==(const iterator &rhs) const
      {
        return (current_block == rhs.current_block) && (index == rhs.index);
      }
      bool operator==(const const_iterator &rhs) const
      {
        return (current_block == rhs.current_block) && (index == rhs.index);
      }
      bool operator!=(const iterator &rhs) const
      {
        return (current_block != rhs.current_block) || (index != rhs.index);
      }
      bool operator!=(const const_iterator &rhs) const
      {
        return (current_block != rhs.current_block) || (index != rhs.index);
      }
    };

    class const_iterator
    {
    public:
      const block *current_block;
      int index;
      const_iterator(block *_current_block = nullptr, int _index = 0) : index(_index), current_block(_current_block) {}
      const_iterator(const iterator &t)
      {
        index = t.index;
        current_block = t.current_block;
      }
      std::pair<const block *, int> GetPos() const
      {
        int num = index + 1;
        const block *tmp = current_block->pre;
        while (tmp != nullptr)
        {
          num += tmp->elements.size;
          tmp = tmp->pre;
        }
        tmp = current_block;
        while (tmp->pre != nullptr)
          tmp = tmp->pre;
        return std::make_pair(tmp, num);
      }

      int operator-(const const_iterator &rhs) const
      {
        std::pair<const block *, int> res1, res2;
        res1 = GetPos();
        res2 = rhs.GetPos();
        if (res1.first != res2.first)
          throw("invalid operation");
        return res1.second - res2.second;
      }
      const_iterator &operator+=(const int &n)
      {
        if (n > 0) // + n
        {
          if (n + index < current_block->elements.size)
            index += n;
          else
          {
            int res = n - (current_block->elements.size - index);
            current_block = current_block->nxt;
            while (current_block != nullptr && current_block->nxt != nullptr && res >= current_block->elements.size)
            {
              res -= current_block->elements.size;
              current_block = current_block->nxt;
            }
            if (current_block->nxt == nullptr && res)
              throw("index_out_of_bound");
            else
              index = res;
          }
        }
        else if (n < 0) // - n
        {
          int res = -n;
          if (index >= res)
            index -= res;
          else
          {
            res = res - index;
            current_block = current_block->pre;
            while (current_block != nullptr && res > current_block->elements.size) // judge nullptr first
            {
              res -= current_block->elements.size;
              current_block = current_block->pre;
            }
            if (current_block == nullptr && res)
              throw("index_out_of_bound");
            else
              index = current_block->elements.get_size() - res;
          }
        }
        return *this;
      }
      const_iterator &operator-=(const int &n)
      {
        if (n > 0)
        {
          if (index >= n)
            index -= n;
          else
          {
            int res = n - index;
            current_block = current_block->pre;
            while (current_block != nullptr && res > current_block->elements.size) // judge nullptr first
            {
              res -= current_block->elements.size;
              current_block = current_block->pre;
            }
            if (current_block == nullptr && res)
              throw("index_out_of_bound");
            else
              index = current_block->elements.size - res;
          }
        }
        else if (n < 0)
        {
          int res = -n;
          if (res + index < current_block->elements.size)
            index += res;
          else
          {
            res = res - (current_block->elements.size - index);
            current_block = current_block->nxt;
            while (current_block->nxt != nullptr && res >= current_block->elements.size)
            {
              res -= current_block->elements.size;
              current_block = current_block->nxt;
            }
            if (current_block->nxt == nullptr && res)
              throw("index_out_of_bound");
            else
              index = res;
          }
        }
        return *this;
      }
      const_iterator operator+(const int &n) const
      {
        const_iterator tmp = *this;
        tmp += n;
        return tmp;
      }
      const_iterator operator-(const int &n) const
      {
        const_iterator tmp = *this;
        tmp -= n;
        return tmp;
      }
      const_iterator &operator++()
      {
        if (current_block->nxt == nullptr) // check end()
          throw("invalid operation");
        if (index + 1 == current_block->elements.get_size())
        {
          index = 0;
          current_block = current_block->nxt;
        }
        else
          index++;
        return *this;
      }

      const_iterator operator++(int)
      {
        const_iterator tmp = *this;
        ++(*this);
        return tmp;
      }

      const_iterator &operator--()
      {
        if (index == 0)
        {
          current_block = current_block->pre;
          if (current_block == nullptr)
            throw("invaid operation");
          index = current_block->elements.get_size() - 1;
        }
        else
          index--;
        return *this;
      }

      const_iterator operator--(int)
      {
        const_iterator tmp = *this;
        --(*this);
        return tmp;
      }

      const T &operator*() const
      {
        if (current_block->elements.arr[current_block->elements.st + index] == nullptr)
          throw("invalid operation");
        return this->current_block->elements.arr[current_block->elements.st + index];
      }

      const T *operator->() const noexcept
      {
        if (current_block->elements.arr[current_block->elements.st + index] == nullptr)
          throw("invalid operation");
        return &(current_block->elements.arr[current_block->elements.st + index]);
      }

      bool operator==(const iterator &rhs) const
      {
        return (current_block == rhs.current_block) && (index == rhs.index);
      }
      bool operator==(const const_iterator &rhs) const
      {
        return (current_block == rhs.current_block) && (index == rhs.index);
      }
      bool operator!=(const iterator &rhs) const
      {
        return (current_block != rhs.current_block) || (index != rhs.index);
      }
      bool operator!=(const const_iterator &rhs) const
      {
        return (current_block != rhs.current_block) || (index != rhs.index);
      }
    };

    deque()
    {
      firstblock = new block();
      lastblock = new block();
      firstblock->nxt = lastblock;
      lastblock->pre = firstblock;
      capacity = 0;
      loadingfactor = 9.0;
    }
    deque(const deque &other)
    {
      capacity = other.capacity;
      loadingfactor = other.loadingfactor;
      firstblock = new block(*other.firstblock);
      block *tmp = other.firstblock->nxt, *_pre = firstblock, *cur;
      while (tmp != nullptr)
      {
        cur = new block(*tmp);
        cur->pre = _pre;
        _pre->nxt = cur;
        _pre = cur;
        tmp = tmp->nxt;
      }
      lastblock = cur;
    }

    ~deque()
    {
      delete this->firstblock;
      capacity = 0;
      firstblock = nullptr;
      lastblock = nullptr;
    }

    deque &operator=(const deque &other)
    {
      if (firstblock == other.firstblock && lastblock == other.lastblock)
        return *this;
      this->~deque();
      capacity = other.capacity;
      loadingfactor = other.loadingfactor;
      firstblock = new block(*other.firstblock);
      block *tmp = other.firstblock->nxt, *_pre = firstblock, *cur;
      while (tmp != nullptr)
      {
        cur = new block(*tmp);
        cur->pre = _pre;
        _pre->nxt = cur;
        _pre = cur;
        tmp = tmp->nxt;
      }
      lastblock = cur;
      return *this;
    }

    double two_pow(double x)
    {
      return 1.0 * x * x;
    }

    T &at(const size_t &pos)
    {
      block *cur = firstblock;
      int res = pos;
      if (res < 0)
        throw("index_out_of_bound");
      while (res >= cur->elements.get_size() && cur != lastblock)
      {
        res -= cur->elements.get_size();
        cur = cur->nxt;
      }
      if (cur == lastblock)
        throw("index_out_of_bound");
      else
        return cur->elements[cur->elements.st + res];
    }

    const T &at(const size_t &pos) const
    {
      const block *cur = firstblock;
      int res = pos;
      if (res < 0)
        throw("index_out_of_bound");
      while (res >= cur->elements.get_size() && cur != lastblock)
      {
        res -= cur->elements.get_size();
        cur = cur->nxt;
      }
      if (cur == lastblock)
        throw("index_out_of_bound");
      else
        return cur->elements[cur->elements.st + res];
    }

    T &operator[](const size_t &pos)
    {
      block *cur = firstblock;
      int res = pos;
      if (res < 0)
        throw("index_out_of_bound");
      while (res >= cur->elements.get_size() && cur != lastblock)
      {
        res -= cur->elements.get_size();
        cur = cur->nxt;
      }
      if (cur == lastblock)
        throw("index_out_of_bound");
      else
        return cur->elements[cur->elements.st + res];
    }

    const T &operator[](const size_t &pos) const
    {
      const block *cur = firstblock;
      int res = pos;
      if (res < 0)
        throw("index_out_of_bound");
      while (res >= cur->elements.get_size() && cur != lastblock)
      {
        res -= cur->elements.get_size();
        cur = cur->nxt;
      }
      if (cur == lastblock)
        throw("index_out_of_bound");
      else
        return cur->elements[cur->elements.st + res];
    }

    const T &front() const
    {
      if (empty())
        throw("container_is_empty");
      else
        return firstblock->elements.front();
    }

    const T &back() const
    {
      if (empty())
        throw("container_is_empty");
      else
        return lastblock->pre->elements.back();
    }

    iterator begin()
    {
      return empty() ? end() : iterator(this->firstblock, 0); // check cap == 0
    }
    const_iterator cbegin() const
    {
      return empty() ? cend() : const_iterator(this->firstblock, 0);
    }

    iterator end()
    {
      return iterator(this->lastblock, 0);
    }
    const_iterator cend() const
    {
      return const_iterator(this->lastblock, 0);
    }

    bool empty() const
    {
      return capacity == 0;
    }

    size_t size() const
    {
      return capacity;
    }

    void clear()
    {
      this->~deque();
      firstblock = new block();
      lastblock = new block();
      firstblock->nxt = lastblock;
      lastblock->pre = firstblock;
      capacity = 0;
      loadingfactor = 9.0;
    }

    block *find(iterator pos)
    {
      block *tmp = pos.current_block;
      while (tmp->pre != nullptr)
        tmp = tmp->pre;
      return tmp;
    }

    void split(block *item)
    {
      block *_nxt = item->nxt;
      block *new_block = new block();
      T **old_arr = new T *[item->elements.cap];
      int num = item->elements.get_size();
      for (int i = 0; i < item->elements.cap; i++)
      {
        if (i < num)
          old_arr[i] = item->elements.arr[i];
        else
          old_arr[i] = nullptr;
      }
      new_block->elements.resize(item->elements.cap >> 1);
      item->elements.resize(item->elements.cap >> 1);
      item->elements.size = num / 2;
      new_block->elements.size = num - item->elements.size;
      for (int i = 0; i < num; i++)
      {
        if (i < num / 2)
          item->elements.arr[i] = old_arr[i];
        else
          new_block->elements.arr[i - (num / 2)] = old_arr[i];
        old_arr[i] = nullptr;
      }
      item->nxt = new_block;
      new_block->pre = item;
      new_block->nxt = _nxt;
      if (_nxt != nullptr)
        _nxt->pre = new_block;
      for (int i = 0; i < num; i++)
        old_arr[i] = nullptr;
      delete[] old_arr;
    }

    void merge(block *block1, block *block2)
    {
      if (block1 == nullptr || block2->nxt == nullptr) // judge firstblock or lastblock
        return;
      block *_nxt = block2->nxt;
      int num = block2->elements.size;
      for (int i = 0; i < num; i++)
        block1->elements.push_back(*block2->elements.arr[i]);
      block1->nxt = _nxt;
      if (_nxt != nullptr)
        _nxt->pre = block1;
      block2->nxt = nullptr, delete block2;
    }

    iterator insert(iterator pos, const T &value)
    {
      if (find(pos) != firstblock)
        throw("invalid operation");
      if (pos.current_block == lastblock)
      {
        lastblock->pre->elements.push_back(value);
        capacity++;
        if (two_pow(lastblock->pre->elements.get_size()) > loadingfactor * capacity)
          split(lastblock->pre);
        return --end();
      }
      else
      {
        pos.current_block->elements.insert(value, pos.index);
        if (two_pow(pos.current_block->elements.get_size()) > loadingfactor * capacity)
          split(pos.current_block);
        capacity++;
        if (pos.index < pos.current_block->elements.size)
          return iterator(pos.current_block, pos.index);
        else
          return iterator(pos.current_block->nxt, pos.index - pos.current_block->elements.size);
      }
    }

    iterator erase(iterator pos)
    {
      if (empty())
        throw("invalid operation");
      pos.current_block->elements.remove(pos.index);
      capacity--;
      iterator res;
      if (two_pow(pos.current_block->elements.get_size()) * loadingfactor < capacity)
      {
        if (pos.current_block->nxt != lastblock)
          merge(pos.current_block, pos.current_block->nxt);
        else if (pos.current_block->pre != nullptr)
        {
          int num = pos.current_block->pre->elements.size;
          block *tmp = pos.current_block->pre;
          merge(pos.current_block->pre, pos.current_block);
          pos.current_block = tmp;
          pos.index = num + pos.index;
        }
      }
      res = pos.index >= pos.current_block->elements.get_size() ? iterator(pos.current_block->nxt, 0) : iterator(pos.current_block, pos.index);
      return res;
    }

    void push_back(const T &value)
    {
      lastblock->pre->elements.push_back(value);
      capacity++;
      if (two_pow(lastblock->pre->elements.get_size()) > loadingfactor * capacity)
        split(lastblock->pre);
    }

    void pop_back()
    {
      if (capacity == 0)
        throw("invalid opeartion");
      lastblock->pre->elements.pop_back();
      capacity--;
      if (two_pow(lastblock->pre->elements.get_size()) * loadingfactor < capacity)
        merge(lastblock->pre->pre, lastblock->pre);
    }

    void push_front(const T &value)
    {
      firstblock->elements.push_front(value);
      capacity++;
      if (two_pow(firstblock->elements.get_size()) > loadingfactor * capacity)
        split(firstblock);
    }

    void pop_front()
    {
      if (capacity == 0)
        throw("invalid operation");
      firstblock->elements.pop_front();
      capacity--;
      if (two_pow(firstblock->elements.get_size()) * loadingfactor < capacity)
        merge(firstblock, firstblock->nxt);
    }
  };

} // namespace sjtu

#endif
