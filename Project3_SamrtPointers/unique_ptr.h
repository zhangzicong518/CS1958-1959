#ifndef UNIQUE_PTR
#define UNIQUE_PTR

template <typename T>
class UniquePtr
{
public:
    UniquePtr();
    explicit UniquePtr(T * pointer);
    ~UniquePtr();

    //The normal copy constructor and the copy assignment operator are deleted
    UniquePtr(UniquePtr& other) = delete;
    UniquePtr& operator= (UniquePtr & other) = delete;

    //Try these: move copy constructor and move assignemnt operator!
    UniquePtr(UniquePtr&& other);
    UniquePtr& operator= (UniquePtr && other) noexcept;

    operator bool() const;
    T* get() const;
    T& operator*() const;
    T* operator->() const;
    void reset();
    void reset(T * new_pointer);
    T* release();
    
private:
    // Add whatever you want to add.
};

//A tricky question: to implement the make_unique function
//template <?>
//? make_unique(?);

#endif //UNIQUE_PTR