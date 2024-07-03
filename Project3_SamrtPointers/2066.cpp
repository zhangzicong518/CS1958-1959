#include <utility>
#ifndef UNIQUE_PTR
#define UNIQUE_PTR

template <typename T>
class UniquePtr
{
public:
    UniquePtr();
    explicit UniquePtr(T *pointer);
    ~UniquePtr();
    UniquePtr(UniquePtr &other) = delete;
    UniquePtr &operator=(UniquePtr &other) = delete;
    UniquePtr(UniquePtr &&other);
    UniquePtr &operator=(UniquePtr &&other) noexcept;
    operator bool() const;
    T *get() const;
    T &operator*() const;
    T *operator->() const;
    void reset();
    void reset(T *new_pointer);
    T *release();

private:
    T *ptr;
};

template <typename T>
UniquePtr<T>::UniquePtr() : ptr(nullptr) {}

template <typename T>
UniquePtr<T>::UniquePtr(T *pointer) : ptr(pointer) {}

template <typename T>
UniquePtr<T>::~UniquePtr()
{
    if (ptr != nullptr)
    {
        delete ptr;
        ptr = nullptr;
    }
}

template <typename T>
UniquePtr<T>::UniquePtr(UniquePtr &&other) : ptr(other.ptr)
{
    other.ptr = nullptr;
}

template <typename T>
UniquePtr<T> &UniquePtr<T>::operator=(UniquePtr &&other) noexcept
{
    if (ptr == other.ptr)
        return *this;
    this->~UniquePtr();
    ptr = other.ptr;
    other.ptr = nullptr;
    return *this;
}

template <typename T>
UniquePtr<T>::operator bool() const
{
    return ptr != nullptr;
}

template <typename T>
T *UniquePtr<T>::get() const
{
    return ptr;
}

template <typename T>
T &UniquePtr<T>::operator*() const
{
    return *ptr;
}

template <typename T>
T *UniquePtr<T>::operator->() const
{
    return ptr;
}

template <typename T>
void UniquePtr<T>::reset()
{
    this->~UniquePtr();
    ptr = nullptr;
}

template <typename T>
void UniquePtr<T>::reset(T *new_pointer)
{
    if (ptr == new_pointer)
        return;
    this->~UniquePtr();
    ptr = new_pointer;
}

template <typename T>
T *UniquePtr<T>::release()
{
    T *pointer = ptr;
    ptr = nullptr;
    return pointer;
}

template <typename T, typename... Ts>
UniquePtr<T> make_unique(Ts &&...list)
{
    return UniquePtr<T>(new T(std::forward<Ts>(list)...));
}

#endif

#include <iostream>

bool TEST()
{
    bool result = 0;
    UniquePtr<double> ptr{make_unique<double>(1.567)};
    UniquePtr<double> ptr1=make_unique<double>(std::move(*ptr));
    if (*ptr1.get() == 1.567  && *ptr.get()!=1.567)
    {
        result = 1;
    }
    return result;
}

int main()
{
    std::cout << TEST() << std::endl;
    return 0;
}
