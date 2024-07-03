#include <utility>
#include <cstdio>
#ifndef SHARED_PTR
#define SHARED_PTR

template <typename T>
class SharedPtr
{
public:
    SharedPtr();
    explicit SharedPtr(T *p);
    ~SharedPtr();
    SharedPtr(const SharedPtr &other);
    SharedPtr &operator=(const SharedPtr &other);
    operator bool() const;
    size_t use_count() const;
    T *get() const;
    T &operator*() const;
    T *operator->() const;
    void reset();
    void reset(T *p);

private:
    size_t *num;
    T *ptr;
};

template <typename T>
SharedPtr<T>::SharedPtr() : num(nullptr), ptr(nullptr) {}

template <typename T>
SharedPtr<T>::SharedPtr(T *p)
{
    if (p == nullptr)
        num = nullptr;
    else
        num = new size_t(1);
    ptr = p;
}

template <typename T>
SharedPtr<T>::~SharedPtr()
{
    if (!ptr)
    {
        num = nullptr;
        return;
    }
    if (num != nullptr && *num >= 0)
        --(*num);
    if (num != nullptr && *num == 0)
    {
        delete ptr;
        delete num;
    }
    ptr = nullptr;
    num = nullptr;
}

template <typename T>
SharedPtr<T>::SharedPtr(const SharedPtr &other)
{
    if (this == &other)
        return;
    num = other.num;
    ptr = other.ptr;
    if (num != nullptr)
        ++(*num);
}

template <typename T>
SharedPtr<T> &SharedPtr<T>::operator=(const SharedPtr &other)
{
    if (this == &other)
        return *this;
    this->~SharedPtr();
    ptr = other.ptr;
    num = other.num;
    if (num != nullptr)
        ++(*num);
    return *this;
}

template <typename T>
SharedPtr<T>::operator bool() const
{
    return nullptr != ptr;
}

template <typename T>
size_t SharedPtr<T>::use_count() const
{
    if (num != nullptr)
        return *num;
    else
        return 0;
}

template <typename T>
T *SharedPtr<T>::get() const
{
    return ptr;
}

template <typename T>
T &SharedPtr<T>::operator*() const
{
    return *ptr;
}

template <typename T>
T *SharedPtr<T>::operator->() const
{
    return ptr;
}

template <typename T>
void SharedPtr<T>::reset()
{
    this->~SharedPtr();
    num = nullptr;
    ptr = nullptr;
}

template <typename T>
void SharedPtr<T>::reset(T *p)
{
    if (ptr == p)
        return;
    this->~SharedPtr();
    ptr = p;
    if (ptr == nullptr)
        num = nullptr;
    else
        num = new size_t(1);
}

template <typename T, typename... Ts>
SharedPtr<T> make_shared(Ts &&...list)
{
    return SharedPtr<T>(new T(std::forward<Ts>(list)...));
}

#endif
