#ifndef SHARED_PTR
#define SHARED_PTR

template <typename T>
class SharedPtr
{
public:
    SharedPtr();
    SharedPtr(T *p);
    ~SharedPtr();
    SharedPtr(const SharedPtr &other);
    SharedPtr &operator=(const SharedPtr &other);
    operator bool();
    size_t use_count() const;
    T *get() const;
    T &operator*() const;
    T *operator->() const;
    void reset();
    void reset(T *p);

private:
    // add whatever u want bro
};

// Then implement the make_shared function
// I think I'd better leave you to design the prototype :)

// template <?>
//? make_shared(?);

#endif // SHARED_PTR