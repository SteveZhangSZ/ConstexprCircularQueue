# Single Header Constexpr Circular Queue
The max capacity must be known at compile time. This requires C++17.

## Getting Started
Simply `#include "circularQueue.hpp"` into the file that you want to use this queue.

### More Info About The Circular Queue
It works for any object of type T where both `std::is_trivially_destructible<T>::value` and 
`std::is_trivially_copy_assignable<T>::value` are true, because in C++17, the destructor must not be user defined
and changing the active member of the union in a constexpr context can only be done through 
defaulted copy assignment. Said assignment only avoids deletion if T is trivially copy assignable.

This queue is composed of a C style array of unions. The union has two members: an empty class and a variable 
whose type depends on the template argument, class T.
```
template<class T, bool B> union Cell; //bool B == std::is_trivially_destructible<T>::value
template<class T>
union Cell<T, true>{
    class emptyClass{} forConstexprCtor;
    T value;
    //Initializes forConstexprCtor because constexpr union constructors must initialize a member
    constexpr Cell() : forConstexprCtor{} {}
    //Initializes value with the provided parameter arguments
    template<typename... Args> 
    constexpr Cell(Args&&... args) : value((args)...) {}
};
template<class T>
union Cell<T, false>{
    class emptyClass{} forConstexprCtor;
    T value;
    constexpr Cell() : forConstexprCtor{} {}
    template<typename... Args> 
    constexpr Cell(Args&&... args) : value((args)...) {}
    ~Cell(){} //Included because Cell<T, false>'s destructor is deleted
};
```
These unions ensure that the storage for the objects of type T will have proper alignment and size.

The entire queue allocates on the stack so it should be faster when you know the size at compile time and are 
working with a small amount of elements.

### Example Usage
```
#include "circularQueue.hpp"
int main(){
    circularQueue<std::string, 3> theStringCQ("First", "Second", "Third");
    constexpr circularQueue<int, 3> theIntCQ(0, 1, 2);
    circularQueue<std::string*, 3> onHeap(new std::string("Heap one"));
    delete onHeap.front();
}
```