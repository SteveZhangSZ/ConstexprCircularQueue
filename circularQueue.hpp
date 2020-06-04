#include <cstddef>
//Modify circular queue so you can pass arguments like this
template<class T ,std::size_t N /*Max capacity*/>
class circularQueue{
    union myUnion{
        bool forConstexprCtor;
        T value;
        constexpr myUnion() : forConstexprCtor{true} {};
    };
    myUnion theArray[N];
    //Head == idx of element at the front. Tail == idx of last element + 1. theSize == queue's size
    std::size_t head, tail, theSize;
    public:
        constexpr circularQueue() : head{0}, tail{0}, theSize{0} {}
    
        template<typename... t>
        constexpr circularQueue(t&&... theList) : head{0}, tail{0}, theSize{sizeof...(theList)}{
            static_assert(sizeof...(theList) <= N, "The number of elements must be less than or equal to N\n");
            theArray[tail++].value = (T(theList),...);
        }
        constexpr bool push(T theObj){
            if(theSize == N){
                return false;//queue is full
            }
            theArray[(tail == N ? tail = 0 : tail++)].value = theObj;
            return ++theSize; //++theSize always > 0. Return true
        }
        template<typename ...Args> 
        constexpr bool emplace(Args&&... args){
            if(theSize == N){
                return false;//queue is full
            }
            theArray[(tail == N ? tail = 0 : tail++)] = T(std::forward<Args>(args)...);
            return ++theSize;
        }

        constexpr const T& front() const noexcept{
            return theArray[head].value;
        }

        constexpr bool pop() noexcept{
            if(!theSize) return false; //If it's empty, pop fails
            (head == N ? head = 0 : head++);
            return theSize--;//Even if theSize == 1, theSize-- will > 0 so this returns true.
        }

        constexpr bool empty() const noexcept{
            return !theSize;
        }
        constexpr std::size_t size() const noexcept{
            return theSize;
        }
        constexpr std::size_t maxCapacity() const noexcept{
            return N;
        }
};