#ifndef CIRCULARQUEUEHPP
#define CIRCULARQUEUEHPP
#include <cstddef>
#include <type_traits>

template<class T ,std::size_t N /*Max capacity*/, 
typename Idxtype = std::size_t /*Integral type to store indices. May change,
like to uint_least16_t, to lower space usage*/,
typename = typename std::enable_if<std::is_integral<Idxtype>::value>::type>
class circularQueue{
    union myUnion{
        bool forConstexprCtor;
        T value;
        constexpr myUnion() : forConstexprCtor{true} {}

        template<class PossibleUnion,typename = typename std::enable_if<std::is_same<PossibleUnion, myUnion>::value >::type>
        constexpr myUnion(PossibleUnion&& other) : value{other.value} {}

        template<typename... Args,typename = typename std::enable_if<std::is_constructible_v<T,Args>...>::type> 
        constexpr myUnion(Args&&... args) : value(std::forward<Args>(args)...) {}

        template<typename AnotherUnion>
        constexpr void operator=(const AnotherUnion&& other){
            value = other.value;
        }
    };
    struct myStruct{
        myUnion theArray[N];
        template<typename... t>
        constexpr myStruct(t&&... theList) : theArray{(theList)...} {}
    } mS;
    //Head == idx of element at the front. Tail == idx of last element + 1. theSize == queue's size
    Idxtype head, tail, theSize;
    
    public:
        constexpr circularQueue() : head{0}, tail{0}, theSize{0} {}
        explicit constexpr circularQueue(const circularQueue<T,N>& other) : mS{other.mS}, head{other.head}, 
        tail{other.tail}, theSize{other.theSize} {}
        
        explicit constexpr circularQueue(circularQueue<T,N>& other) : 
        circularQueue{const_cast<circularQueue<T,N> const&>(other)} {}
        
        template<typename... Args/*, typename = typename std::enable_if<std::is_constructible_v<myStruct, Args>... >::type*/ >
        explicit constexpr circularQueue(Args&&... theList) : mS{(theList)...}, head{0},
        tail{sizeof...(theList)}, theSize{sizeof...(theList)}{}
    
        constexpr bool push(const T theObj){
            if(theSize == N){
                return false;//queue is full
            }
            mS.theArray[(tail == N ? (tail = 0)++ : tail++)] = myUnion(std::move(theObj));
            return ++theSize; //++theSize always > 0. Return true
        }
        template<typename ...Args> 
        constexpr bool emplace(Args&&... args){
            if(theSize == N){
                return false;//queue is full
            }
            mS.theArray[(tail == N ? (tail = 0)++ : tail++)] = myUnion(std::forward<Args>(args)...);
            return ++theSize;
        }

        constexpr const T& front() const noexcept{
            return mS.theArray[head].value;
        }

        constexpr bool pop() noexcept{
            if(!theSize) return false; //If it's empty, pop fails
            (head == N ? head = 0 : head++);
            return theSize--;//Even if theSize == 1, theSize-- will > 0 so this returns true.
        }

        constexpr bool empty() const noexcept{
            return !theSize;
        }
        constexpr Idxtype size() const noexcept{
            return theSize;
        }
        constexpr std::size_t maxCapacity() const noexcept{
            return N;
        }
        //Assignment
        constexpr circularQueue& operator=(const circularQueue<T,N>& other){ 
            std::size_t first{head = other.head};
            tail = other.tail;
            theSize = other.theSize;
            if(other.tail < other.head){ //Only need to copy elements from other.head to other.tail
                for(; first < N; ++first){
                    mS.theArray[first] = other.mS.theArray[first];
                }
                for(first = 0; first < tail; ++first){
                    mS.theArray[first] = other.mS.theArray[first];
                }
            }
            else{
                for(; first < other.tail; ++first) mS.theArray[first] = other.mS.theArray[first];
            }
            return *this;
        }
        constexpr circularQueue& operator=(const circularQueue<T,N>&& other){ 
            std::size_t first{head = std::move(other.head)};
            tail = std::move(other.tail);
            theSize = std::move(other.theSize);
            if(other.tail < other.head){ //Only need to copy elements from other.head to other.tail
                for(; first < N; ++first){
                    mS.theArray[first] = std::move(other.mS.theArray[first]);
                }
                for(first = 0; first < tail; ++first){
                    mS.theArray[first] = std::move(other.mS.theArray[first]);
                }
            }
            else{
                for(; first < other.tail; ++first) mS.theArray[first] = std::move(other.mS.theArray[first]);
            }
            return *this;
        }
};
#endif //CIRCULARQUEUEHPP