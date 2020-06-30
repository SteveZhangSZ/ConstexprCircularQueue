#ifndef CIRCULARQUEUEHPP
#define CIRCULARQUEUEHPP
#include <cstddef>
#include <type_traits>

template<class T ,std::size_t N /*Max capacity*/, 
typename Idxtype = std::size_t /*Integral type to store indices. May change,
like to uint_least16_t, to lower space usage*/>
class circularQueue{
    static_assert(std::is_integral<Idxtype>::value, "Idxtype must be an integral type.");
    union myUnion{
        class emptyStruct{} forConstexprCtor;
        T value;
        //Default constructor for myUnion. Initializes the dummy class forConstexprCtor for constexpr-ness
        constexpr myUnion() : forConstexprCtor{} {}
        //constexpr myUnion(T theT) : value{std::move(theT)} {};

        //template<class PossibleUnion,typename = typename std::enable_if<std::is_same<PossibleUnion, myUnion>::value >::type>
        //constexpr myUnion(PossibleUnion&& other) : value{other.value} {}
    /*
        constexpr myUnion(const myUnion& other) : value{other.value} {}
        constexpr myUnion(myUnion& other) : value{other.value} {}
        constexpr myUnion(myUnion&& other)  : value{std::move(other.value)} {}
    */
        template<typename... Args,typename = typename std::enable_if<std::is_constructible_v<T,Args>...>::type> 
        constexpr myUnion(Args&&... args) : value(std::forward<Args>(args)...) {}
/*
            if constexpr(std::is_same<AnotherUnion, myUnion&&>::value){
                value = std::move(other.value);
            } else {
                value = other.value;
            }
            */
    /*
        template<typename AnotherUnion>constexpr void operator=(AnotherUnion&& other){
            if constexpr(std::is_same<AnotherUnion, myUnion&&>::value){
                value = std::move(other.value);
            } else if constexpr(std::is_same<AnotherUnion,const myUnion&>::value){
                value = other.value;
            } else{
                value = other.value;
            }
        }
    */
        //~myUnion(){}
     /*   
        constexpr void operator=(const myUnion& other){value = other.value;}
        constexpr void operator=(myUnion& other){value = other.value;}
        constexpr void operator=(myUnion&& other){value = std::move(other.value);}
    */
    /*
        constexpr void operator=(const T& other){*this = myUnion(other);}
        constexpr void operator=(T& other){*this = myUnion(other);}
        constexpr void operator=(T&& other){*this = myUnion(other);}
    */
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
        constexpr circularQueue(const circularQueue<T,N>& other) : mS{other.mS}, head{other.head}, 
        tail{other.tail}, theSize{other.theSize} {}
        
        constexpr circularQueue(circularQueue<T,N>& other) : circularQueue{const_cast<circularQueue<T,N> const&>(other)} {}
        
        constexpr circularQueue(circularQueue<T,N>&& other) : mS{std::move(other.mS)}, head{std::move(other.head)}, 
        tail{std::move(other.tail)}, theSize{std::move(other.theSize)} {}

        template<typename... Args/*, typename = typename std::enable_if<std::is_constructible_v<myStruct, Args>... >::type*/ >
        explicit constexpr circularQueue(Args&&... theList) : mS{(theList)...}, head{0},
        tail{sizeof...(theList)}, theSize{sizeof...(theList)}{}
    
        //For push and emplace, if the 
        //data member has a non-trivial special member function (copy/move constructor, copy/move assignment, 
        //or destructor), that function is deleted by default in the union and needs to be defined explicitly 
        //by the programmer.
        //template<class U>
        constexpr bool push(T theObj){
            //static_assert(std::is_same<typename std::decay<U>::type, T>::value || std::is_convertible<U,T>::value );
            if(theSize == N){
                return false;//queue is full
            }
            mS.theArray[(tail == N ? (tail = 0)++ : tail++)] = myUnion(std::move(theObj));
            //mS.theArray[(tail == N ? (tail = 0)++ : tail++)].value = std::move(theObj);
            return ++theSize; //++theSize always > 0. Return true
        }
        template<typename ...Args> 
        constexpr bool emplace(Args&&... args){
            if(theSize == N){
                return false;//queue is full
            }
            mS.theArray[(tail == N ? (tail = 0)++ : tail++)] = myUnion(std::forward<Args>(args)...);
            //mS.theArray[(tail == N ? (tail = 0)++ : tail++)].value = T(std::forward<Args>(args)...);
            return ++theSize;
        }

        constexpr const T& front() const{
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
        constexpr std::size_t capacity() const noexcept{
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
                for(; first < other.tail; ++first){
                    mS.theArray[first] = other.mS.theArray[first];
                }
            }
            return *this;
        }
        constexpr circularQueue& operator=(circularQueue<T,N>&& other){ 
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
                for(; first < other.tail; ++first){ 
                    mS.theArray[first] = std::move(other.mS.theArray[first]);
                }
            }
            return *this;
        }
};
#endif //CIRCULARQUEUEHPP