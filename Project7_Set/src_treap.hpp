#ifndef MY_SET
#define MY_SET

#include <stdexcept>

template <class Key, class Compare = std::less<Key>>
class ESet
{
    friend class iterator;

private:
    enum rotate_dir
    {
        Right = 0,
        Left = 1
    };
    struct node
    {
        Key val;
        node *child[2];
        int siz, rnd;
        node(const Key &_val, node *_left = nullptr, node *_right = nullptr, int _siz = 1) : val(_val), siz(_siz)
        {
            rnd = rand();
            child[0] = _left;
            child[1] = _right;
        }
        ~node() {}
    };
    mutable node *root;

    void clear(node *n)
    {
        if (n == nullptr)
            return;
        clear(n->child[0]);
        clear(n->child[1]);
        delete n;
        n = nullptr;
    }

    void copy_tree(node *n, node *&x)
    {
        if (n == nullptr)
            return;
        x = new node(n->val);
        x->siz = n->siz;
        copy_tree(n->child[0], x->child[0]);
        copy_tree(n->child[1], x->child[1]);
    }

    void pushup(node *now) const
    {
        if (now == nullptr)
            return;
        now->siz = (now->child[0] == nullptr ? 0 : now->child[0]->siz) + (now->child[1] == nullptr ? 0 : now->child[1]->siz) + 1;
    }

    void rotate(node *&cur, rotate_dir dir)
    {
        node *tmp = cur->child[dir];
        cur->child[dir] = tmp->child[!dir];
        tmp->child[!dir] = cur;
        pushup(cur);
        pushup(tmp);
        cur = tmp;
    }

public:
    class iterator
    {
        friend class ESet;

    public:
        const node *ptr;
        ESet *container;
        bool is_end;

        iterator(node *_ptr = nullptr, ESet *_container = nullptr, bool _is_end = false) : ptr(_ptr), container(_container), is_end(_is_end) {}

        iterator(const iterator &other)
        {
            ptr = other.ptr;
            container = other.container;
            is_end = other.is_end;
        }
        ~iterator()
        {
            ptr = nullptr;
            container = nullptr;
        }

        iterator &operator=(const iterator &other)
        {
            if (this == &other)
                return *this;
            ptr = other.ptr;
            container = other.container;
            is_end = other.is_end;
            return *this;
        }

        iterator operator++(int)
        {
            iterator itr(*this);
            ++(*this);
            return itr;
        }

        iterator &operator++()
        {
            if (is_end)
                return *this;
            node *tmp = container->root, *_ptr = const_cast<node *>(ptr);
            while (tmp)
            {
                if (Compare()(ptr->val, tmp->val))
                {
                    _ptr = tmp;
                    tmp = tmp->child[0];
                }
                else
                    tmp = tmp->child[1];
            }
            if (_ptr == ptr)
            {
                ptr = nullptr;
                is_end = true;
            }
            else
                ptr = _ptr;
            return *this;
        }

        iterator operator--(int)
        {
            iterator itr(*this);
            --(*this);
            return itr;
        }

        iterator &operator--()
        {
            if (is_end)
            {
                is_end = false;
                ptr = container->find_max(container->root);
                return *this;
            }
            node *tmp = container->root, *_ptr = const_cast<node *>(ptr);
            while (tmp)
            {
                if (Compare()(tmp->val, ptr->val))
                {
                    _ptr = tmp;
                    tmp = tmp->child[1];
                }
                else
                    tmp = tmp->child[0];
            }
            ptr = _ptr;
            return *this;
        }

        const Key &operator*() const
        {
            return ptr->val;
        }
        bool operator==(const iterator &rhs) const
        {
            return ptr == rhs.ptr && container == rhs.container && is_end == rhs.is_end;
        }
        bool operator!=(const iterator &rhs) const
        {
            return ptr != rhs.ptr || container != rhs.container || is_end != rhs.is_end;
        }
    };

    ESet()
    {
        root = nullptr;
    }

    ESet(const ESet &other)
    {
        root = nullptr;
        copy_tree(other.root, root);
    }

    ESet &operator=(const ESet &other)
    {
        if (root == other.root)
            return *this;
        clear();
        copy_tree(other.root, root);
        return *this;
    }

    ESet(ESet &&other)
    {
        root = nullptr;
        std::swap(root, other.root);
    }

    ESet &operator=(ESet &&other) noexcept
    {
        if (other.root == root)
            return *this;
        clear();
        std::swap(root, other.root);
        return *this;
    }

    ~ESet()
    {
        clear();
    }

    iterator begin() const
    {
        node *tmp = find_min(root);
        if (tmp == nullptr)
            return end();
        else
            return iterator(tmp, const_cast<ESet *>(this), false);
    }

    iterator end() const
    {
        return iterator(nullptr, const_cast<ESet *>(this), true);
    }

    size_t size() const
    {
        return root == nullptr ? 0 : root->siz;
    }

    node *find_min(node *cur) const
    {
        node *tmp = cur;
        while (tmp && tmp->child[0])
            tmp = tmp->child[0];
        return tmp;
    }

    node *find_max(node *cur) const
    {
        node *tmp = cur;
        while (tmp && tmp->child[1])
            tmp = tmp->child[1];
        return tmp;
    }

    void clear()
    {
        clear(root);
        root = nullptr;
    }

    template <class... Args>
    std::pair<iterator, bool> emplace(Args &&...args)
    {
        return insert(std::forward<Args>(args)...);
    }

    template <class... Args>
    std::pair<iterator, bool> insert(const Key &key, Args &...args)
    {
        iterator res = find(key);
        if (res == end())
        {
            node *tmp = insert_node(root, key);
            res = iterator(tmp, this, false);
            return std::make_pair(res, true);
        }
        else
            return std::make_pair(res, false);
    }

    node *insert_node(node *&cur, const Key &key)
    {
        if (cur == nullptr)
        {
            cur = new node(key);
            return cur;
        }
        else if (Compare()(key, cur->val))
        {
            node *tmp = insert_node(cur->child[0], key);
            if (cur->child[0]->rnd < cur->rnd)
                rotate(cur, Right);
            pushup(cur);
            return tmp;
        }
        else
        {
            node *tmp = insert_node(cur->child[1], key);
            if (cur->child[1]->rnd < cur->rnd)
                rotate(cur, Left);
            pushup(cur);
            return tmp;
        }
    }

    size_t erase(const Key &key)
    {
        iterator res = find(key);
        if (res == end())
            return 0;
        else
        {
            erase_node(root, key);
            return 1;
        }
    }

    void erase_node(node *&cur, const Key &key)
    {
        if (Compare()(cur->val, key))
        {
            erase_node(cur->child[1], key);
            pushup(cur);
        }
        else if (Compare()(key, cur->val))
        {
            erase_node(cur->child[0], key);
            pushup(cur);
        }
        else
        {
            int state = 0;
            state |= (cur->child[0] != nullptr);
            state |= ((cur->child[1] != nullptr) << 1);
            node *tmp = cur;
            switch (state)
            {
            case 0:
                delete cur;
                cur = nullptr;
                break;
            case 1:
                cur = tmp->child[0];
                delete tmp;
                break;
            case 2:
                cur = tmp->child[1];
                delete tmp;
                break;
            case 3:
                rotate_dir dir = cur->child[0]->rnd < cur->child[1]->rnd ? Right : Left;
                rotate(cur, dir);
                erase_node(cur->child[!dir], key);
                pushup(cur);
                break;
            }
        }
    }

    iterator find(const Key &key) const
    {
        node *tmp = root;
        while (tmp != nullptr)
        {
            if (Compare()(tmp->val, key))
                tmp = tmp->child[1];
            else if (Compare()(key, tmp->val))
                tmp = tmp->child[0];
            else
                break;
        }
        if (tmp == nullptr)
            return end();
        else
            return iterator(tmp, const_cast<ESet *>(this), false);
    }

    int query_rank(const Key &key) const
    {
        int res = 1;
        node *tmp = root;
        while (tmp != nullptr && (Compare()(tmp->val, key) || Compare()(key, tmp->val)))
        {
            if (Compare()(key, tmp->val))
                tmp = tmp->child[0];
            else
            {
                res += (tmp->child[0] == nullptr ? 0 : tmp->child[0]->siz) + 1;
                tmp = tmp->child[1];
            }
        }
        if (tmp != nullptr)
            res += tmp->child[0] == nullptr ? 0 : tmp->child[0]->siz;
        return res;
    }

    size_t range(const Key &l, const Key &r) const
    {
        if (l > r)
            return 0;
        int rnum = query_rank(r) + (find(r) == end() ? -1 : 0);
        int lnum = query_rank(l) - 1;
        return rnum - lnum;
    }

    iterator lower_bound(const Key &key) const
    {
        node *tmp = root, *fa = nullptr;
        while (tmp != nullptr)
        {
            if (Compare()(tmp->val, key))
                tmp = tmp->child[1];
            else
            {
                fa = tmp;
                tmp = tmp->child[0];
            }
        }
        return fa == nullptr ? end() : iterator(fa, const_cast<ESet *>(this), false);
    }

    iterator upper_bound(const Key &key) const
    {
        node *tmp = root, *fa = nullptr;
        while (tmp != nullptr)
        {
            if (Compare()(key, tmp->val))
            {
                fa = tmp;
                tmp = tmp->child[0];
            }
            else
                tmp = tmp->child[1];
        }
        return fa == nullptr ? end() : iterator(fa, const_cast<ESet *>(this), false);
    }
};

#endif
