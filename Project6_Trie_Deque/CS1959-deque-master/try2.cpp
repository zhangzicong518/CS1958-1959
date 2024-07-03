#pragma once

template <typename T, int max_size = 10>
class LimVec
{
    T *arr[max_size] = {nullptr};
    int size;

    void swap(int a, int b);

public:
    bool is_empty();
    bool is_filled();
    int get_size();

    void push_back(T elem);
    void push_pointer_front(T *elem);
    void push_front(T elem);

    template <typename... Args>
    void emplace_back(Args &&...args);
    template <typename... Args>
    void emplace_front(Args &&...args);
    template <typename... Args>
    void emplace(int pos, Args &&...args);

    T &operator[](int pos);
    T &at(int pos);

    T &front();
    T &back();
    void nullout_last();

    void insert_before(T elem, int pos);
    void remove_at(int pos);

    void pop_front();
    void pop_back();

    LimVec();
    ~LimVec();
};

template <typename T, int max_size>
void LimVec<T, max_size>::swap(int a, int b)
{
    T *temp = arr[a];
    arr[a] = arr[b];
    arr[b] = temp;
}

template <typename T, int max_size>
bool LimVec<T, max_size>::is_empty()
{
    return size == 0;
}

template <typename T, int max_size>
bool LimVec<T, max_size>::is_filled()
{
    return size == max_size;
}

template <typename T, int max_size>
int LimVec<T, max_size>::get_size()
{
    return this->size;
}

template <typename T, int max_size>
void LimVec<T, max_size>::push_back(T elem)
{
    if (size < max_size)
        arr[++size - 1] = new T(elem);
}

template <typename T, int max_size>
void LimVec<T, max_size>::push_pointer_front(T *elem)
{
    if (size < max_size)
    {
        size++;
        for (int k = size - 1; k > 0; k--)
        {
            swap(k, k - 1);
        }
        arr[0] = elem;
    }
}

template <typename T, int max_size>
void LimVec<T, max_size>::push_front(T elem)
{
    if (size < max_size)
    {
        size++;
        for (int k = size - 1; k > 0; k--)
        {
            swap(k, k - 1);
        }
        arr[0] = new T(elem);
    }
}

template <typename T, int max_size>
T &LimVec<T, max_size>::operator[](int pos)
{
    if (pos < size)
        return *(arr[pos]);
}

template <typename T, int max_size>
T &LimVec<T, max_size>::at(int pos)
{
    if (pos < size)
        return *(arr[pos]);
}

template <typename T, int max_size>
T &LimVec<T, max_size>::front()
{
    return *arr[0];
}

template <typename T, int max_size>
T &LimVec<T, max_size>::back()
{
    return *arr[size - 1];
}

template <typename T, int max_size>
void LimVec<T, max_size>::nullout_last()
{
    arr[--size] = nullptr;
}

template <typename T, int max_size>
void LimVec<T, max_size>::insert_before(T elem, int pos)
{
    if (size < max_size)
    {
        size++;
        for (int k = size - 1; k != pos; k--)
            arr[k] = arr[k - 1];
        arr[pos] = new T(elem);
    }
}

template <typename T, int max_size>
void LimVec<T, max_size>::remove_at(int pos)
{
    if (pos < size)
    {
        delete arr[pos];
        arr[pos] = nullptr;
        size--;
        for (int k = pos; k < size - 1; k++)
            swap(k, k + 1);
    }
}

template <typename T, int max_size>
void LimVec<T, max_size>::pop_front()
{
    delete arr[0];
    arr[0] = nullptr;
    for (int k = 0; k < size; k++)
        swap(k, k + 1);
    --size;
}

template <typename T, int max_size>
void LimVec<T, max_size>::pop_back()
{
    delete arr[--size];
    arr[size] = nullptr;
}

template <typename T, int max_size>
LimVec<T, max_size>::LimVec()
{
    size = 0;
}

template <typename T, int max_size>
LimVec<T, max_size>::~LimVec()
{
    for (int i = 0; i < size; i++)
    {
        delete arr[i];
    }
}

template <typename T, int max_size>
template <typename... Args>
void LimVec<T, max_size>::emplace_back(Args &&...args)
{
    if (size < max_size)
        arr[++size - 1] = new T(std::forward<Args>(args)...);
}

template <typename T, int max_size>
template <typename... Args>
void LimVec<T, max_size>::emplace_front(Args &&...args)
{
    if (size < max_size)
    {
        size++;
        for (int k = size - 1; k > 0; k--)
        {
            swap(k, k - 1);
        }
        arr[0] = new T(std::forward<Args>(args)...);
    }
}

template <typename T, int max_size>
template <typename... Args>
void LimVec<T, max_size>::emplace(int pos, Args &&...args)
{
    if (size < max_size)
    {
        size++;
        for (int k = size - 1; k != pos; k--)
            arr[k] = arr[k - 1];
        arr[pos] = new T(std::forward<Args>(args)...);
    }
}

template <typename T, int max_size = 10>
class Deque
{
private:
    struct Node;

    Node *first_node;
    Node *last_node;

    int size;

public:
    struct DequeItr;

    Deque();
    ~Deque();

    T &front();
    T &back();

    typename Deque<T, max_size>::DequeItr begin();
    typename Deque<T, max_size>::DequeItr end();
    const typename Deque<T, max_size>::DequeItr cbegin();
    const typename Deque<T, max_size>::DequeItr cend();

    T &operator[](int pos);
    T &at(int pos);

    void push_back(T elem);
    void push_front(T elem);
    void insert(Deque<T, max_size>::DequeItr pos, T elem);

    template <typename... Args>
    void emplace_back(Args &&...args);
    template <typename... Args>
    void emplace_front(Args &&...args);
    template <typename... Args>
    void emplace(Deque<T, max_size>::DequeItr pos, Args &&...args);

    void pop_front();
    void pop_back();
    void erase(Deque<T, max_size>::DequeItr pos);
};

template <typename T, int max_size>
struct Deque<T, max_size>::Node
{
    LimVec<T, max_size> elements;
    Node *next_node;
    Node *prev_node;

    Node();
    ~Node();
    Node(Node *_next_node, Node *_prev_node);
};

template <typename T, int max_size>
Deque<T, max_size>::Deque()
{
    first_node = new Node();
    last_node = new Node();
    first_node->next_node = last_node;
    last_node->prev_node = first_node;
}

template <typename T, int max_size>
Deque<T, max_size>::~Deque()
{
    delete this->first_node;
}

template <typename T, int max_size>
T &Deque<T, max_size>::front()
{
    return first_node->elements.front();
}

template <typename T, int max_size>
T &Deque<T, max_size>::back()
{
    return last_node->prev_node->elements.back();
}

template <typename T, int max_size>
T &Deque<T, max_size>::operator[](int pos)
{
    Node *curr_node = first_node;
    while (pos >= curr_node->elements.get_size())
    {
        pos -= curr_node->elements.get_size();
        curr_node = curr_node->next_node;
    }
    return curr_node->elements[pos];
}

template <typename T, int max_size>
T &Deque<T, max_size>::at(int pos)
{
    Node *curr_node = first_node;
    while (pos >= curr_node->elements.get_size())
    {
        pos -= curr_node->elements.get_size();
        curr_node = curr_node->next_node;
    }
    return curr_node->elements[pos];
}

template <typename T, int max_size>
void Deque<T, max_size>::push_back(T elem)
{
    if (last_node->prev_node->elements.is_filled())
    {
        last_node->prev_node->next_node = new Node(last_node, last_node->prev_node);
        last_node->prev_node = last_node->prev_node->next_node;
        last_node->prev_node->elements.push_back(elem);
    }
    else
    {
        last_node->prev_node->elements.push_back(elem);
    }
    ++size;
}

template <typename T, int max_size>
void Deque<T, max_size>::push_front(T elem)
{
    if (first_node->elements.is_filled())
    {
        first_node->prev_node = new Node(first_node, nullptr);
        first_node = first_node->prev_node;
        first_node->elements.push_front(elem);
    }
    else
    {
        first_node->elements.push_front(elem);
    }
    ++size;
}

template <typename T, int max_size>
void Deque<T, max_size>::insert(Deque<T, max_size>::DequeItr pos, T elem)
{
    if (pos.this_node->elements.is_filled() || pos.this_node->next_node == last_node)
    {
        T *last_elem = &pos.this_node->elements.back();
        pos.this_node->elements.nullout_last();
        if (pos.this_node->next_node->elements.is_filled())
        {
            Node *temp_node = new Node(pos.this_node->next_node, pos.this_node);
            pos.this_node->next_node->prev_node = temp_node;
            pos.this_node->next_node = temp_node;
            pos.this_node->elements.insert_before(elem, pos.index);
            temp_node->elements.push_pointer_front(last_elem);
        }
        else
        {
            pos.this_node->next_node->elements.push_pointer_front(last_elem);
            pos.this_node->elements.insert_before(elem, pos.index);
        }
    }
    else
    {
        pos.this_node->elements.insert_before(elem, pos.index);
    }
    ++size;
}

template <typename T, int max_size>
void Deque<T, max_size>::pop_front()
{
    first_node->elements.pop_front();
    if (first_node->elements.is_empty())
    {
        Node *temp_ptr = first_node;
        first_node = first_node->next_node;
        temp_ptr->next_node = nullptr;
        delete temp_ptr;
    }
    --size;
}

template <typename T, int max_size>
void Deque<T, max_size>::pop_back()
{
    last_node->prev_node->elements.pop_back();
    if (last_node->prev_node->elements.is_empty())
    {
        Node *temp_node = last_node->prev_node;
        temp_node->prev_node->next_node = last_node;
        last_node->prev_node = temp_node->prev_node;
        temp_node->next_node = nullptr;
        delete temp_node;
    }
    --size;
}

template <typename T, int max_size>
void Deque<T, max_size>::erase(Deque<T, max_size>::DequeItr pos)
{
    pos.this_node->elements.remove_at(pos.index);
    if (pos.this_node->elements.is_empty())
    {
        Node *temp_node = pos.this_node;
        temp_node->next_node->prev_node = temp_node->prev_node;
        if (temp_node->prev_node)
            temp_node->prev_node->next_node = temp_node->next_node;
        else
            this->first_node = temp_node->next_node;
        temp_node->next_node = nullptr;
        delete temp_node;
    }
    --size;
}

template <typename T, int max_size>
Deque<T, max_size>::Node::Node()
{
    next_node = nullptr;
    prev_node = nullptr;
}

template <typename T, int max_size>
Deque<T, max_size>::Node::~Node()
{
    delete this->next_node;
}

template <typename T, int max_size>
Deque<T, max_size>::Node::Node(Node *_next_node, Node *_prev_node)
{
    next_node = _next_node;
    prev_node = _prev_node;
}

template <typename T, int max_size>
struct Deque<T, max_size>::DequeItr
{
    Node *this_node;
    int index;

    Deque<T, max_size>::DequeItr &operator++();
    Deque<T, max_size>::DequeItr &operator++(int);

    Deque<T, max_size>::DequeItr &operator--();
    Deque<T, max_size>::DequeItr &operator--(int);

    bool operator==(const Deque<T, max_size>::DequeItr &itr);
    bool operator!=(const Deque<T, max_size>::DequeItr &itr);

    T &operator*();

    DequeItr();
    DequeItr(Node *new_node, int _index = 0);
    ~DequeItr();
};

template <typename T, int max_size>
typename Deque<T, max_size>::DequeItr &Deque<T, max_size>::DequeItr::operator++()
{
    if (this->index + 1 == this->this_node->elements.get_size())
    {
        this->index = 0;
        this->this_node = this->this_node->next_node;
    }
    else
    {
        this->index++;
    }
    return *this;
}

template <typename T, int max_size>
typename Deque<T, max_size>::DequeItr &Deque<T, max_size>::DequeItr::operator++(int)
{
    if (this->index + 1 == this->this_node->elements.get_size())
    {
        this->index = 0;
        this->this_node = this->this_node->next_node;
    }
    else
    {
        this->index++;
    }
    return *this;
}

template <typename T, int max_size>
typename Deque<T, max_size>::DequeItr &Deque<T, max_size>::DequeItr::operator--()
{
    if (this->index == 0)
    {
        this->this_node = this->this_node->prev_node;
        index = this->this_node->elements.get_size() - 1;
    }
    else
    {
        this->index--;
    }
    return *this;
}

template <typename T, int max_size>
typename Deque<T, max_size>::DequeItr &Deque<T, max_size>::DequeItr::operator--(int)
{
    if (this->index == 0)
    {
        this->this_node = this->this_node->prev_node;
        index = this->this_node->elements.get_size() - 1;
    }
    else
    {
        this->index--;
    }
    return *this;
}

template <typename T, int max_size>
bool Deque<T, max_size>::DequeItr::operator==(const Deque<T, max_size>::DequeItr &itr)
{
    return (this->this_node == itr.this_node) && (this->index == itr.index);
}

template <typename T, int max_size>
bool Deque<T, max_size>::DequeItr::operator!=(const Deque<T, max_size>::DequeItr &itr)
{
    return !(*this == itr);
}

template <typename T, int max_size>
T &Deque<T, max_size>::DequeItr::operator*()
{
    return this_node->elements[index];
}

template <typename T, int max_size>
Deque<T, max_size>::DequeItr::DequeItr()
{
    this_node = nullptr;
    index = 0;
}

template <typename T, int max_size>
Deque<T, max_size>::DequeItr::DequeItr(Node *new_node, int _index)
{
    this_node = new_node;
    index = _index;
}

template <typename T, int max_size>
Deque<T, max_size>::DequeItr::~DequeItr(){};

template <typename T, int max_size>
typename Deque<T, max_size>::DequeItr Deque<T, max_size>::begin()
{
    return typename Deque<T, max_size>::DequeItr(this->first_node, 0);
}

template <typename T, int max_size>
typename Deque<T, max_size>::DequeItr Deque<T, max_size>::end()
{
    return Deque<T, max_size>::DequeItr(this->last_node, 0);
}

template <typename T, int max_size>
const typename Deque<T, max_size>::DequeItr Deque<T, max_size>::cbegin()
{
    return Deque<T, max_size>::DequeItr(this->first_node, 0);
}

template <typename T, int max_size>
const typename Deque<T, max_size>::DequeItr Deque<T, max_size>::cend()
{
    return Deque<T, max_size>::DequeItr(this->last_node, 0);
}

template <typename T, int max_size>
template <typename... Args>
void Deque<T, max_size>::emplace_back(Args &&...args)
{
    if (last_node->prev_node->elements.is_filled())
    {
        last_node->prev_node->next_node = new Node(last_node, last_node->prev_node);
        last_node->prev_node = last_node->prev_node->next_node;
        last_node->prev_node->elements.emplace_back(std::forward(args)...);
    }
    else
    {
        last_node->prev_node->elements.emplace_back(std::forward(args)...);
    }
    ++size;
}

template <typename T, int max_size>
template <typename... Args>
void Deque<T, max_size>::emplace_front(Args &&...args)
{
    if (first_node->elements.is_filled())
    {
        first_node->prev_node = new Node(first_node, nullptr);
        first_node = first_node->prev_node;
        first_node->elements.emplace_front(std::forward<Args>(args)...);
    }
    else
    {
        first_node->elements.emplace_front(std::forward<Args>(args)...);
    }
    ++size;
}

template <typename T, int max_size>
template <typename... Args>
void Deque<T, max_size>::emplace(Deque<T, max_size>::DequeItr pos, Args &&...args)
{
    if (pos.this_node->elements.is_filled() || pos.this_node->next_node == last_node)
    {
        T *last_elem = &pos.this_node->elements.back();
        pos.this_node->elements.nullout_last();
        if (pos.this_node->next_node->elements.is_filled())
        {
            Node *temp_node = new Node(pos.this_node->next_node, pos.this_node);
            pos.this_node->next_node->prev_node = temp_node;
            pos.this_node->next_node = temp_node;
            pos.this_node->elements.emplace(pos.index, std::forward<Args>(args)...);
            temp_node->elements.push_pointer_front(last_elem);
        }
        else
        {
            pos.this_node->next_node->elements.push_pointer_front(last_elem);
            pos.this_node->elements.emplace(pos.index, std::forward<Args>(args)...);
        }
    }
    else
    {
        pos.this_node->elements.emplace(pos.index, std::forward<Args>(args)...);
    }
    ++size;
}