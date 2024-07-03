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
        node *left, *right, *parent;
        int siz;
        bool is_end, is_red;
        node(const Key &_val, node *_left = nullptr, node *_right = nullptr, node *_parent = nullptr, bool _is_red = true, bool _is_end = false, int _siz = 1) : val(_val), left(_left), right(_right), parent(_parent), is_red(_is_red), is_end(_is_end), siz(_siz) {}
        ~node() {}
    };
    node *root;
    node *header;
    size_t node_count;

    void init()
    {
        void *tmp = operator new(sizeof(node));
        header = reinterpret_cast<node *>(tmp);
        header->parent = root;
        header->left = header;
        header->right = header;
        header->is_end = true;
        header->is_red = true;
        header->siz = 0;
    }

    void clear(node *n)
    {
        if (n == nullptr)
            return;
        clear(n->left);
        clear(n->right);
        delete n;
        n = nullptr;
    }

    node *find_min(node *x)
    {
        while (x->left != nullptr)
            x = x->left;
        return x;
    }

    node *find_max(node *x)
    {
        while (x->right != nullptr)
            x = x->right;
        return x;
    }

    void rotate_left(node *x)
    {
        node *y = x->right;
        x->right = y->left;
        if (y->left != nullptr)
            y->left->parent = x;
        y->parent = x->parent;
        if (x == root)
        {
            root = y;
            header->parent = y;
            root->parent = header;
        }
        else if (x == x->parent->left)
            x->parent->left = y;
        else
            x->parent->right = y;
        y->left = x;
        x->parent = y;
        pushup(x);
    }

    void rotate_right(node *y)
    {
        node *x = y->left;
        y->left = x->right;
        if (x->right != nullptr)
            x->right->parent = y;
        x->parent = y->parent;
        if (y == root) // insert pos == root
        {
            root = x;
            header->parent = x;
            root->parent = header;
        }
        else if (y == y->parent->left)
            y->parent->left = x;
        else
            y->parent->right = x;
        x->right = y;
        y->parent = x;
        pushup(y);
    }

    void after_insert_fix(node *x)
    {
        x->is_red = true;
        while (x != root && x->parent->is_red == true)
        {
            if (x->parent == x->parent->parent->left)
            {
                node *y = x->parent->parent->right;
                if (y != nullptr && y->is_red == true)
                {
                    x->parent->is_red = false;
                    y->is_red = false;
                    x->parent->parent->is_red = true;
                    x = x->parent->parent;
                }
                else
                {
                    if (x == x->parent->right)
                    {
                        x = x->parent;
                        rotate_left(x);
                    }
                    x->parent->is_red = false;
                    x->parent->parent->is_red = true;
                    rotate_right(x->parent->parent);
                }
            }
            else
            {
                node *y = x->parent->parent->left;
                if (y != nullptr && y->is_red == true)
                {
                    x->parent->is_red = false;
                    y->is_red = false;
                    x->parent->parent->is_red = true;
                    x = x->parent->parent;
                }
                else
                {
                    if (x == x->parent->left)
                    {
                        x = x->parent;
                        rotate_right(x);
                    }
                    x->parent->is_red = false;
                    x->parent->parent->is_red = true;
                    rotate_left(x->parent->parent);
                }
            }
        }
        root->is_red = false;
    }

    node *erase_node(node *z)
    {
        node *y = z;
        node *x = nullptr;
        node *x_parent = nullptr;
        if (y->left == nullptr)
            x = y->right;
        else if (y->right == nullptr)
            x = y->left;
        else
        {
            y = y->right;
            while (y->left != nullptr)
                y = y->left;
            x = y->right;
        }
        // erase_node has two children
        // equal to find nxt of the erase_node
        // and swap them
        if (y != z)
        {
            z->left->parent = y;
            y->left = z->left;
            if (y != z->right)
            {
                x_parent = y->parent;
                if (x)
                    x->parent = y->parent;
                y->parent->left = x;
                y->right = z->right;
                z->right->parent = y;
            }
            else
                x_parent = y;
            if (root == z)
                root = y;
            else if (z->parent->left == z)
                z->parent->left = y;
            else
                z->parent->right = y;
            y->parent = z->parent;
            std::swap(y->is_red, z->is_red);
            y = z;
        }
        else // has <= 1 child
        {
            x_parent = y->parent;
            if (x)
                x->parent = y->parent;
            if (root == z)
                root = x;
            else if (z->parent->left == z)
                z->parent->left = x;
            else
                z->parent->right = x;
            if (header->left == z) // update the mininum of the tree
                if (z->right == nullptr)
                    header->left = z->parent;
                else
                    header->left = find_min(x);
            if (header->right == z)
                if (z->left == nullptr)
                    header->right = z->parent;
                else
                    header->right = find_max(x);
        }
        if (x)
            pushup(x);
        else if (x_parent)
            pushup(x_parent);
        if (y->is_red != true)
            after_erase_fix(x, x_parent);
        return y;
    }

    void after_erase_fix(node *x, node *x_parent)
    {
        while (x != root && (x == nullptr || x->is_red == false))
        {
            if (x == x_parent->left)
            {
                node *w = x_parent->right; // sibling of the current node
                if (w->is_red == true)
                {
                    w->is_red = false;
                    x_parent->is_red = true;
                    rotate_left(x_parent);
                    w = x_parent->right;
                }
                if ((w->left == nullptr ||
                     w->left->is_red == false) &&
                    (w->right == nullptr ||
                     w->right->is_red == false)) // case 1 has no children or two black children
                {
                    w->is_red = true;
                    x = x_parent;
                    x_parent = x_parent->parent;
                }
                else
                {
                    if (w->right == nullptr || w->right->is_red == false) // case 2 has no other side child or with black color
                    {
                        if (w->left != nullptr)
                            w->left->is_red = false;
                        w->is_red = true;
                        rotate_right(w);
                        w = x_parent->right;
                    }
                    // case 3 otherwise
                    w->is_red = x_parent->is_red;
                    x_parent->is_red = false;
                    if (w->right != nullptr)
                        w->right->is_red = false;
                    rotate_left(x_parent);
                    break;
                }
            }
            else
            {
                node *w = x_parent->left;
                if (w->is_red == true)
                {
                    w->is_red = false;
                    x_parent->is_red = true;
                    rotate_right(x_parent);
                    w = x_parent->left;
                }
                if ((w->right == nullptr || w->right->is_red == false) &&
                    (w->left == nullptr ||
                     w->left->is_red == false))
                {
                    w->is_red = true;
                    x = x_parent;
                    x_parent = x_parent->parent;
                }
                else
                {
                    if (w->left == nullptr || w->left->is_red == false)
                    {
                        if (w->right != nullptr)
                            w->right->is_red = false;
                        w->is_red = true;
                        rotate_left(w);
                        w = x_parent->left;
                    }
                    w->is_red = x_parent->is_red;
                    x_parent->is_red = false;
                    if (w->left != nullptr)
                        w->left->is_red = false;
                    rotate_right(x_parent);
                    break;
                }
            }
        }
        if (x != nullptr)
            x->is_red = false;
    }

    void copy_tree(node *n, node *&x, node *y)
    {
        if (n == nullptr)
            return;
        x = new node(n->val);
        x->parent = y;
        x->is_end = n->is_end;
        x->is_red = n->is_red;
        x->siz = n->siz;
        copy_tree(n->left, x->left, x);
        copy_tree(n->right, x->right, x);
    }

    void pushup(node *now)
    {
        if (now == nullptr || now == header || now->is_end)
            return;
        now->siz = (now->left == nullptr ? 0 : now->left->siz) + (now->right == nullptr ? 0 : now->right->siz) + 1;
        pushup(now->parent); // pushup back to root
    }

public:
    class iterator
    {
        friend class ESet;

    public:
        const node *ptr;
        const ESet *container;

        iterator(node *p = nullptr, const ESet *c = nullptr) : ptr(p), container(c)
        {
        }
        iterator(const iterator &other)
        {
            ptr = other.ptr;
            container = other.container;
        }
        ~iterator() {}

        iterator &operator=(const iterator &rhs)
        {
            if (this == &rhs)
                return *this;
            ptr = rhs.ptr;
            container = rhs.container;
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
            if (ptr == container->header) // end()++
                return *this;
            else
            {
                if (ptr->right != nullptr)
                {
                    ptr = ptr->right;
                    while (ptr->left != nullptr)
                        ptr = ptr->left;
                }
                else // back to root find nxt
                {
                    node *tmp = ptr->parent;
                    while (ptr == tmp->right)
                    {
                        ptr = tmp;
                        tmp = tmp->parent;
                    }
                    if (ptr->right != tmp)
                    {
                        ptr = tmp;
                    }
                }
                return *this;
            }
        }

        iterator operator--(int)
        {
            iterator itr(*this);
            --(*this);
            return itr;
        }

        iterator &operator--()
        {
            if (ptr == container->header->left) // begin()--
                return *this;
            else
            {
                node *tmp;
                if (ptr->is_end) // end()--
                    ptr = ptr->right;
                else if (ptr->left != nullptr)
                {
                    tmp = ptr->left;
                    while (tmp->right != nullptr)
                        tmp = tmp->right;
                    ptr = tmp;
                }
                else // back to root to fiind pre
                {
                    tmp = ptr->parent;
                    while (ptr == tmp->left)
                    {
                        ptr = tmp;
                        tmp = tmp->parent;
                    }
                    ptr = tmp;
                }
                return *this;
            }
        }
        const Key &operator*() const
        {
            return ptr->val;
        }
        bool operator==(const iterator &rhs) const
        {
            return ptr == rhs.ptr && container == rhs.container;
        }
        bool operator!=(const iterator &rhs) const
        {
            return ptr != rhs.ptr || container != rhs.container;
        }
    };

    ESet()
    {
        root = nullptr;
        init();
        node_count = 0;
    }

    ESet(const ESet &other)
    {
        root = nullptr;
        init();
        node_count = other.node_count;
        copy_tree(other.root, root, header);
        if (root != nullptr)
        {
            node *tmp = root;
            while (tmp->left != nullptr)
            {
                tmp = tmp->left;
            }
            header->left = tmp;
            node *tmp2 = root;
            while (tmp2->right != nullptr)
            {
                tmp2 = tmp2->right;
            }
            header->right = tmp2;
        }
        header->parent = root;
    }

    ESet &operator=(const ESet &other)
    {
        if (this == &other)
            return *this;
        clear();
        node_count = other.node_count;
        copy_tree(other.root, root, header);
        if (root != nullptr) // modify minimum and maximum
        {
            node *tmp = root;
            while (tmp->left != nullptr)
            {
                tmp = tmp->left;
            }
            header->left = tmp;
            node *tmp2 = root;
            while (tmp2->right != nullptr)
            {
                tmp2 = tmp2->right;
            }
            header->right = tmp2;
        }
        header->parent = root;
        return *this;
    }

    ESet(ESet &&other) // swap pointer
    {
        root = nullptr;
        init();
        std::swap(root, other.root);
        std::swap(node_count, other.node_count);
        std::swap(header, other.header);
    }

    ESet &operator=(ESet &&other) noexcept
    {
        if (other.header == header)
            return *this;
        clear();
        root = nullptr;
        std::swap(root, other.root);
        std::swap(node_count, other.node_count);
        std::swap(header, other.header);
        return *this;
    }

    ~ESet()
    {
        clear();
        operator delete(header);
        header = nullptr;
    }

    iterator begin() const
    {
        iterator itr(header->left, this);
        return itr;
    }

    iterator end() const
    {
        iterator itr(header, this);
        return itr;
    }

    size_t size() const
    {
        return node_count;
    }

    void clear()
    {
        clear(root);
        header->left = header;
        header->right = header;
        header->parent = root = nullptr;
        node_count = 0;
    }

    template <class... Args>
    std::pair<iterator, bool> emplace(Args &&...args)
    {
        return insert(std::forward<Args>(args)...);
    }

    template <class... Args>
    std::pair<iterator, bool> insert(const Key &key, Args &...args)
    {
        node *y = header;
        node *x = root;
        bool comp = true;
        while (x != nullptr)
        {
            y = x;
            comp = Compare()(key, x->val);
            x = comp ? x->left : x->right;
        }
        iterator j(y, this); // loacte where to insert
        if (comp)
        {
            if (j == begin())
                return std::make_pair(insert_node(key, x, y), true);
            else
                --j;
        }
        if (Compare()(j.ptr->val, key)) // not exist
            return std::make_pair(insert_node(key, x, y), true);
        return std::make_pair(j, false); // already exist
    }

    size_t erase(const Key &key)
    {
        iterator res = find(key);
        if (res == end())
            return 0;
        else
        {
            erase_node(res);
            return 1;
        }
    }

    void erase_node(iterator pos)
    {
        if (pos.ptr == nullptr || const_cast<node *>(pos.ptr) == header || pos.container != this)
            return;
        else
        {
            node *y = erase_node(const_cast<node *>(pos.ptr));
            delete y;
            --node_count;
        }
    }

    iterator find(const Key &key) const
    {
        node *y = header;
        node *x = root;
        while (x != nullptr)
        {
            if (Compare()(x->val, key))
                x = x->right;
            else
            {
                y = x;
                x = x->left;
            }
        }
        iterator j = iterator(y, this);
        return (j == end() || Compare()(key, j.ptr->val)) ? end() : j;
    }

    int rank(const Key &key) const // rankk = the number of node samller than key
    {
        int res = 1;
        node *tmp = root;
        while (tmp != nullptr && (Compare()(tmp->val, key) || Compare()(key, tmp->val)))
        {
            if (Compare()(key, tmp->val))
                tmp = tmp->left;
            else
            {
                res += (tmp->left == nullptr ? 0 : tmp->left->siz) + 1;
                tmp = tmp->right;
            }
        }
        if (tmp != nullptr)
            res += tmp->left == nullptr ? 0 : tmp->left->siz;
        return res;
    }

    size_t range(const Key &l, const Key &r) const
    {
        if (l > r)
            return 0;
        int rnum = rank(r) + (find(r) == end() ? -1 : 0);
        int lnum = rank(l) - 1;
        return rnum - lnum;
    }

    iterator lower_bound(const Key &key) const
    {
        node *tmp = root, *fa = header;
        while (tmp != nullptr)
        {
            if (Compare()(tmp->val, key))
                tmp = tmp->right;
            else
            {
                fa = tmp;
                tmp = tmp->left;
            }
        }
        return iterator(fa, this);
    }

    iterator upper_bound(const Key &key) const
    {
        node *tmp = root, *fa = header;
        while (tmp != nullptr)
        {
            if (Compare()(key, tmp->val))
            {
                fa = tmp;
                tmp = tmp->left;
            }
            else
                tmp = tmp->right;
        }
        return iterator(fa, this);
    }

private:
    iterator insert_node(const Key &key, node *x, node *y)
    {
        node *z;
        if (y == header || x != nullptr || Compare()(key, y->val)) // insert left
        {
            y->left = new node(key);
            z = y->left;
            if (y == header)
            {
                root = z;
                y->right = z;
                y->parent = z;
                root->parent = header;
            }
            else if (y == header->left)
                header->left = z;
        }
        else
        {
            y->right = new node(key);
            z = y->right;
            if (y == header->right)
                header->right = z;
        }
        pushup(y);
        z->parent = y;
        after_insert_fix(z);
        ++node_count;
        return iterator(z, this);
    }
};

#endif