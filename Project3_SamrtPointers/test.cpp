#include <utility>
#include <cstring>
#include <vector>
#include <iostream>

using namespace std;

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

class myclass
{
public:
    int a, b, c;
    string d;

    myclass(int a1, int b1, int c1, string d1) : a(a1), b(b1), c(c1), d(d1) {}
};

bool TEST11(double &result)
{
    UniquePtr<myclass> ptr1{new myclass{1, 2, 3, "1"}};
    UniquePtr<myclass> ptr2(std::move(ptr1));
    UniquePtr<myclass> ptr3;
    ptr3 = std::move(ptr2);
    if (ptr1.get() != nullptr || ptr2.get() != nullptr || ptr3->b != 2 || (*ptr3).d != "1" || ptr3.get()->c != 3)
    {
        result = 0;
    }
    UniquePtr<myclass> ptr5 = make_unique<myclass>(1, 2, 3, "1");
    UniquePtr<myclass> ptr6(make_unique<myclass>(1, 2, 3, "1"));
    if (ptr5->b != 2 || (*ptr5).d != "1" || ptr5.get()->c != 3 || ptr6->b != 2 || (*ptr6).d != "1")
    {
        result = 0;
    }
    ptr6 = std::move(ptr5);
    if (ptr5.get() != nullptr || ptr6->b != 2 || (*ptr6).d != "1" || ptr6.get()->c != 3)
    {
        result = 0;
    }
    UniquePtr<std::vector<int>> ptr7 = make_unique<std::vector<int>>();
    if (!ptr7->empty())
    {
        result = 0;
    }
    ptr7->push_back(1);
    if (ptr7->empty())
    {
        result = 0;
    }
    UniquePtr<myclass> ptr8 = make_unique<myclass>(*ptr6);
    if (ptr8->b != 2 || ptr8->c != 3 || ptr8->d != "1")
    {
        result = 0;
    }
    UniquePtr<UniquePtr<myclass>> ptr9 =
        make_unique<UniquePtr<myclass>>(std::move(make_unique<myclass>(1, 2, 3, "1")));
    if ((*ptr9)->b != 2 || (*ptr9)->c != 3 || (*ptr9)->d != "1")
    {
        result = 0;
    }
    ptr9.reset();
    if (ptr9.get() != nullptr)
    {
        result = 0;
    }
    ptr9 = make_unique<UniquePtr<myclass>>(std::move(ptr8));
    if (ptr8.get() != nullptr || (*ptr9)->b != 2 || (*ptr9)->c != 3 || (*ptr9)->d != "1")
    {
        result = 0;
    }
    if ((*ptr9.release())->b != 2)
    {
        result = 0;
    }
    return result;
}

int main()
{
    double score = 1;
    TEST11(score);
    cout << score << endl;
    return 0;
}