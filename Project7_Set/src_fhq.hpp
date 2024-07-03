#ifndef MY_SET
#define MY_SET

#include <stdexcept>

template <class Key, class Compare = std::less<Key>>
class ESet
{
    friend class iterator;

private:
    struct node
    {
        Key val;
        node *left, *right;
        int siz, rnd;
        node(const Key &_val, node *_left = nullptr, node *_right = nullptr, int _siz = 1) : val(_val), left(_left), right(_right), siz(_siz)
        {
            rnd = rand();
        }
        ~node() {}
    };
    mutable node *root;

    void clear(node *n)
    {
        if (n == nullptr)
            return;
        clear(n->left);
        clear(n->right);
        delete n;
        n = nullptr;
    }

    void copy_tree(node *n, node *&x)
    {
        if (n == nullptr)
            return;
        x = new node(n->val);
        x->siz = n->siz;
        copy_tree(n->left, x->left);
        copy_tree(n->right, x->right);
    }

    void pushup(node *now) const
    {
        if (now == nullptr)
            return;
        now->siz = (now->left == nullptr ? 0 : now->left->siz) + (now->right == nullptr ? 0 : now->right->siz) + 1;
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
            auto tmp = container->split_by_val(container->root, ptr->val, 0);
            ptr = container->find_min(tmp.second);
            is_end = (ptr == nullptr);
            container->root = container->merge(tmp.first, tmp.second);
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
            auto tmp = container->split_by_val(container->root, ptr->val, 1);
            ptr = container->find_max(tmp.first);
            if (ptr == nullptr)
                ptr = container->find_min(tmp.second);
            container->root = container->merge(tmp.first, tmp.second);
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
        while (tmp && tmp->left)
            tmp = tmp->left;
        return tmp;
    }

    node *find_max(node *cur) const
    {
        node *tmp = cur;
        while (tmp && tmp->right)
            tmp = tmp->right;
        return tmp;
    }

    void clear()
    {
        clear(root);
        root = nullptr;
    }

    // opt = 0 : ( <= key , > key )
    // opt = 1 : ( < key , >= key )
    std::pair<node *, node *> split_by_val(node *cur, const Key &key, int opt = 0) const
    {
        if (cur == nullptr)
            return std::make_pair(nullptr, nullptr);
        if (opt == 0)
        {
            if (Compare()(key, cur->val)) // key < cur->val
            {
                auto tmp = split_by_val(cur->left, key, 0);
                cur->left = tmp.second;
                pushup(cur);
                return std::make_pair(tmp.first, cur);
            }
            else
            {
                auto tmp = split_by_val(cur->right, key, 0);
                cur->right = tmp.first;
                pushup(cur);
                return std::make_pair(cur, tmp.second);
            }
        }
        else
        {
            if (Compare()(cur->val, key)) // cur->val < k
            {
                auto tmp = split_by_val(cur->right, key, 1);
                cur->right = tmp.first;
                pushup(cur);
                return std::make_pair(cur, tmp.second);
            }
            else
            {
                auto tmp = split_by_val(cur->left, key, 1);
                cur->left = tmp.second;
                pushup(cur);
                return std::make_pair(tmp.first, cur);
            }
        }
    }

    node *merge(node *x, node *y) const
    {
        if (x == nullptr && y == nullptr)
            return nullptr;
        else if (x == nullptr && y != nullptr)
            return y;
        else if (x != nullptr && y == nullptr)
            return x;
        if (x->rnd < y->rnd)
        {
            x->right = merge(x->right, y);
            pushup(x);
            return x;
        }
        else
        {
            y->left = merge(x, y->left);
            pushup(y);
            return y;
        }
    }

    template <class... Args>
    std::pair<iterator, bool> emplace(Args &&...args)
    {
        return insert(std::forward<Args>(args)...);
    }

    template <class... Args>
    std::pair<iterator, bool> insert(const Key &key, Args &...args)
    {
        auto tmp1 = split_by_val(root, key, 0);
        auto tmp2 = split_by_val(tmp1.first, key, 1);
        node *l = tmp2.first, *mid = tmp2.second, *r = tmp1.second, *new_node = nullptr, *tmp;
        if (mid == nullptr)
        {
            new_node = new node(key);
            tmp = merge(l, new_node);
            root = merge(tmp, r);
            iterator res(new_node, this, false);
            return std::make_pair(res, true);
        }
        else
        {
            tmp = merge(l, mid);
            root = merge(tmp, r);
            iterator res(mid, this, false);
            return std::make_pair(res, false);
        }
    }

    size_t erase(const Key &key)
    {
        auto tmp1 = split_by_val(root, key, 0);
        auto tmp2 = split_by_val(tmp1.first, key, 1);
        node *l = tmp2.first, *mid = tmp2.second, *r = tmp1.second, *tmp;
        root = merge(l, r);
        if (mid == nullptr)
            return 0;
        else
        {
            delete mid;
            return 1;
        }
    }

    iterator find(const Key &key) const
    {
        auto tmp1 = split_by_val(root, key, 0);
        auto tmp2 = split_by_val(tmp1.first, key, 1);
        node *l = tmp2.first, *mid = tmp2.second, *r = tmp1.second, *tmp;
        tmp = merge(l, mid);
        root = merge(tmp, r);
        iterator res = mid == nullptr ? end() : iterator(mid, const_cast<ESet *>(this), false);
        return res;
    }

    size_t range(const Key &l, const Key &r) const
    {
        if (l > r)
            return 0;
        auto tmp1 = split_by_val(root, r, 0);
        auto tmp2 = split_by_val(tmp1.first, l, 1);
        node *lef = tmp2.first, *mid = tmp2.second, *rig = tmp1.second, *tmp;
        int res = mid == nullptr ? 0 : mid->siz;
        tmp = merge(lef, mid);
        root = merge(tmp, rig);
        return res;
    }

    iterator lower_bound(const Key &key) const
    {
        auto tmp = split_by_val(root, key, 1);
        node *pos = find_min(tmp.second);
        root = merge(tmp.first, tmp.second);
        iterator res = pos == nullptr ? end() : iterator(pos, const_cast<ESet *>(this), false);
        return res;
    }

    iterator upper_bound(const Key &key) const
    {
        auto tmp = split_by_val(root, key, 0);
        node *pos = find_min(tmp.second);
        root = merge(tmp.first, tmp.second);
        iterator res = pos == nullptr ? end() : iterator(pos, const_cast<ESet *>(this), false);
        return res;
    }
};

#endif
