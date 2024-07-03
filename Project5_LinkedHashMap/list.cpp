#include <bits/stdc++.h>
using namespace std;
#define value_type pair<int, int>

struct node
{
    node *pre, *nxt;
    value_type *val;
    node(node *_pre = nullptr, node *_nxt = nullptr, value_type *_val = nullptr) : pre(_pre), nxt(_nxt), val(_val) {}
};

class double_list
{
public:
    node *head, *tail;
    size_t len;
    double_list()
    {
        len = 0;
        head = new node();
        tail = new node();
        head->nxt = tail;
        tail->pre = head;
    }
    double_list(const double_list &other)
    {
        clear();
        for (double_list::iterator it = other.begin(); it != other.end(); it++)
            insert_head(*(it));
    }
    ~double_list()
    {
        node *now = head, *tmp;
        while (now != nullptr)
        {
            tmp = now->nxt;
            delete now->val;
            delete now;
            now = tmp;
        }
        len = 0;
        head = nullptr;
        tail = nullptr;
    }

    class const_iterator;
    class iterator
    {
    public:
        node *iter;
        iterator(node *pos = nullptr)
        {
            iter = pos;
        }
        iterator(const iterator &t)
        {
            iter = t.iter;
        }
        ~iterator()
        {
            iter = nullptr;
        }
        iterator &operator=(const iterator &t)
        {
            iter = t.iter;
        }
        iterator &operator++()
        {
            iter = iter->nxt;
            return *this;
        }
        iterator &operator--()
        {
            iter = iter->pre;
            return *this;
        }
        iterator operator++(int)
        {
            iterator tmp = *this;
            ++(*this);
            return tmp;
        }
        iterator operator--(int)
        {
            iterator tmp = *this;
            --(*this);
            return tmp;
        }
        value_type &operator*() const
        {
            if (iter->val == nullptr)
                throw("invalid");
            return *(iter->val);
        }
        value_type *operator->() const noexcept
        {
            if (iter->val == nullptr)
                throw("invalid");
            return iter->val;
        }
        bool operator==(const iterator &rhs) const
        {
            return iter == rhs.iter;
        }
        bool operator!=(const iterator &rhs) const
        {
            return iter != rhs.iter;
        }
    };
    class const_iterator
    {
    public:
        const node *const_iter;
        const_iterator(node *pos = nullptr)
        {
            const_iter = pos;
        }
        const_iterator(const iterator &other)
        {
            const_iter = other.const_iter;
        }
        const_iterator &operator=(const iterator &other)
        {
            const_iter = other.iter;
        }
        const_iterator &operator++()
        {
            const_iter = const_iter->nxt;
            return *this;
        }
        const_iterator &operator--()
        {
            const_iter = const_iter->pre;
            return *this;
        }
        const_iterator operator++(int)
        {
            iterator tmp = *this;
            ++(*this);
            return tmp;
        }
        const_iterator operator--(int)
        {
            iterator tmp = *this;
            --(*this);
            return tmp;
        }

        const value_type &operator*() const
        {
            if (const_iter->val == nullptr)
                throw("star invalid");
            return *(const_iter->val);
        }
        const value_type *operator->() const noexcept
        {
            if (const_iter->val == nullptr)
                throw("star invalid");
            return *(const_iter->val);
        }

        /**
         * operator to check whether two iterators are same (pointing to the same memory).
         */
        bool operator==(const iterator &rhs) const
        {
            return const_iter == rhs.iter;
        }
        bool operator!=(const iterator &rhs) const
        {
            return const_iter != rhs.iter;
        }
        bool operator==(const const_iterator &rhs) const
        {
            return const_iter == rhs.iter;
        }
        bool operator!=(const const_iterator &rhs) const
        {
            return const_iter != rhs.iter;
        }
    };

    iterator begin()
    {
        return iterator(head->nxt);
    }
    iterator end()
    {
        return iterator(tail);
    }
    const_iterator cbegin() const
    {
        return const_iterator(head->nxt);
    }
    const_iterator cend() const
    {
        return const_iterator(tail);
    }

    iterator erase(iterator pos)
    {
        if (len == 0 || pos == end())
            return pos;
        node *now = pos.iter, *_pre = pos.iter->pre, *_nxt = pos.iter->nxt;
        len--;
        _pre->nxt = _nxt;
        _nxt->pre = _pre;
        return iterator(_nxt);
    }

    void insert(iterator pos, const value_type &val)
    {
        node *tmp = new node();
        tmp->val = new value_type(val);
        len++;
        node *now = pos.iter;
        tmp->pre = now->pre;
        tmp->nxt = now;
        // if (now->pre != nullptr)
        //     now->pre->nxt = tmp;
        now->pre = tmp;
    }

    void insert_head(const value_type &val)
    {
        insert(begin(), val);
    }
    void insert_tail(const value_type &val)
    {
        insert(end(), val);
    }
    void delete_head()
    {
        erase(begin());
    }
    void delete_tail()
    {
        erase(iterator(tail->pre));
    }
    bool empty()
    {
        return (len == 0);
    }

    void clear()
    {
        len = 0;
        head = new node();
        tail = new node();
        head->nxt = tail;
        tail->pre = head;
    }
};

int main()
{

    return 0;
}