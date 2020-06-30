#ifndef CIRCULARQUEUEHPP
#define CIRCULARQUEUEHPP
#include <cstddef>
#include <new> //For placement new
#include <type_traits>
template<class T, bool B> union Cell;//bool B == std::is_trivially_destructible<T>::value
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
template<class T, std::size_t N, typename Idxtype>
struct commonQueueFunctions{
    static_assert(std::is_integral<Idxtype>::value, "Idxtype must be an integral type\n");
    constexpr bool full() const noexcept {return theSize == N;} //Check if queue is full
    constexpr bool empty() const noexcept {return !theSize;} //Check if queue is empty
    constexpr Idxtype size() const noexcept {return theSize;} //Returns the queue's current size
    //Returns the max number of elements the queue may hold
    constexpr std::size_t capacity() const noexcept {return N;}
    //Returns the element next to be popped. Undefined behavior if queue is empty
    constexpr const T& front() const {return theArray[head].value;}
    constexpr T& front() {return theArray[head].value;}
    //Returns the element last to be popped. Undefined behavior if queue is empty
    constexpr const T& back() const {return theArray[tail - 1].value;}
    constexpr T& back() {return theArray[tail - 1].value;}
    protected:
        Idxtype head{0}, tail{0}, theSize{0};
        Cell<T, std::is_trivially_destructible<T>::value> theArray[N];
        constexpr void clear(){ //Destroys value in the queue when value is the active member
            if(this->head > this->tail|| (this->head == this->tail && this->theSize == N)){
                for(; this->head < N; ++this->head){
                    this->theArray[this->head].value.~T();
                }
                this->head = 0;
            } 
            for(; this->head < this->tail; ++this->head){
                this->theArray[this->head].value.~T();
            }
        }
        constexpr commonQueueFunctions() = default;
        constexpr commonQueueFunctions(const commonQueueFunctions& other) : head{other.head},
        tail{other.tail}, theSize(other.theSize){ //Copy constructor
            std::size_t originalHead(other.head);
            //If other is full, there's a chance that other.head == other.tail
            if(other.head > other.tail || (other.head == other.tail && other.theSize == N)){
                for(; originalHead < N; ++originalHead){
                    if constexpr(std::is_trivially_copy_assignable<T>::value && 
                                 std::is_trivially_destructible<T>::value){
                        theArray[originalHead] = other.theArray[originalHead];
                    } else {
                        new(&theArray[originalHead].value)T(other.theArray[originalHead].value);
                    }
                }
                originalHead = 0;
            }
            for(; originalHead < other.tail; ++originalHead){
                if constexpr(std::is_trivially_copy_assignable<T>::value && 
                                 std::is_trivially_destructible<T>::value){
                    theArray[originalHead] = other.theArray[originalHead];
                } else {
                    new(&theArray[originalHead].value)T(other.theArray[originalHead].value);
                }
            }
        }
        constexpr commonQueueFunctions(commonQueueFunctions&& other) : head{other.head},
        tail{std::move(other.tail)}, theSize(std::move(other.theSize)){ //Move constructor
            std::size_t originalHead(std::move(other.head));
            if(other.head > other.tail || (other.head == other.tail && other.theSize == N)){
                for(; originalHead < N; ++originalHead){
                    if constexpr(std::is_trivially_copy_assignable<T>::value && 
                                 std::is_trivially_destructible<T>::value){
                        theArray[originalHead] = std::move(other.theArray[originalHead]);
                    } else {
                        new(&theArray[originalHead].value)T(std::move(other.theArray[originalHead].value));
                    }
                }
                originalHead = 0;
            }
            for(; originalHead < other.tail; ++originalHead){
                if constexpr(std::is_trivially_copy_assignable<T>::value && 
                                 std::is_trivially_destructible<T>::value){
                    theArray[originalHead] = std::move(other.theArray[originalHead]);
                } else {
                    new(&theArray[originalHead].value)T(std::move(other.theArray[originalHead].value));
                }
            }
        }
        constexpr commonQueueFunctions& operator=(const commonQueueFunctions& other){//Copy assignment
            std::size_t originalHead(head = other.head);
            if constexpr((std::is_trivially_copy_assignable<T>::value && 
            std::is_trivially_destructible<T>::value) == false){
                clear();
            } 
            if(other.head > other.tail || (other.head == other.tail && other.theSize == N)){
                for(; originalHead < N; ++originalHead){
                    if constexpr(std::is_trivially_copy_assignable<T>::value && 
                                 std::is_trivially_destructible<T>::value){
                        theArray[originalHead] = other.theArray[originalHead];
                    } else {
                        new(&theArray[originalHead].value)T(other.theArray[originalHead].value);
                    }
                }
                originalHead = 0;
            }
            for(; originalHead < other.tail; ++originalHead){
                if constexpr(std::is_trivially_copy_assignable<T>::value && 
                                 std::is_trivially_destructible<T>::value){
                    theArray[originalHead] = other.theArray[originalHead];
                } else {
                    new(&theArray[originalHead].value)T(other.theArray[originalHead].value);
                }
            }
            tail = other.tail;
            theSize = other.theSize;
            return *this;
        }
        constexpr commonQueueFunctions& operator=(commonQueueFunctions&& other){ //Move assignment
            std::size_t originalHead(head = std::move(other.head));
            if constexpr((std::is_trivially_copy_assignable<T>::value && 
            std::is_trivially_destructible<T>::value) == false){
                clear();
            }
            if(other.head > other.tail || (other.head == other.tail && other.theSize == N)){
                for(; originalHead < N; ++originalHead){
                    if constexpr(std::is_trivially_copy_assignable<T>::value && 
                                 std::is_trivially_destructible<T>::value){
                        theArray[originalHead] = std::move(other.theArray[originalHead]);
                    } else {
                        new(&theArray[originalHead].value)T(std::move(other.theArray[originalHead].value));
                    }
                }
                originalHead = 0;
            }
            for(; originalHead < other.tail; ++originalHead){
                if constexpr(std::is_trivially_copy_assignable<T>::value && 
                                 std::is_trivially_destructible<T>::value){
                    theArray[originalHead] = std::move(other.theArray[originalHead]);
                } else {
                    new(&theArray[originalHead].value)T(std::move(other.theArray[originalHead].value));
                }
            }
            tail = std::move(other.tail);
            theSize = std::move(other.theSize);
            return *this;
        }
        template<typename... Args> //Constructor which accepts arguments to construct theArray
        constexpr commonQueueFunctions(std::size_t theHead, std::size_t theTail, std::size_t paramSize,
        Args&&... theList) : head(theHead), tail(theTail), theSize(paramSize),theArray{(theList)...}{}
};
template<class T, std::size_t N, bool B, typename Idxtype> struct theQueue;
template<class T, std::size_t N, typename Idxtype>
struct theQueue<T,N, true, Idxtype> : public commonQueueFunctions<T, N, Idxtype>{
    constexpr theQueue() = default; //Default constructor
    //Constructor which accepts arguments to construct theArray
    template<typename... Args, typename = 
    typename std::enable_if<(... && std::is_constructible_v<T,Args>)>::type > 
    explicit constexpr theQueue(Args&&... theList) : commonQueueFunctions<T, N, Idxtype>(0, sizeof...(theList),
    sizeof...(theList),std::forward<Args>(theList)...){}
    constexpr bool push(T theObj){//Pushes the given element value to the end of the queue
        if(this->theSize == N){
            return false;//queue is full
        }
        this->theArray[(this->tail == N ? (this->tail = 0)++ : this->tail++)] = Cell<T,true>(std::move(theObj));
        return ++this->theSize; //++theSize always > 0. Return true
    }
    template<typename ...Args> 
    constexpr bool emplace(Args&&... args){ //Same as push, but the element is constructed in-place
        if(this->theSize == N){
            return false;//queue is full
        }
        this->theArray[(this->tail == N ? (this->tail = 0)++ : this->tail++)] = Cell<T,true>((args)...);
        return ++this->theSize;
    }
    constexpr bool pop() noexcept{ //Removes the element at the queue's front
        if(!this->theSize) return false; //If it's empty, pop fails
        (this->head == N ? this->head = 0 : ++this->head);
        return this->theSize--;//Even if theSize == 1, theSize-- will > 0 so this returns true.
    }
};
template<class T, std::size_t N, typename Idxtype>
struct theQueue<T,N, false, Idxtype>  : public commonQueueFunctions<T, N, Idxtype>{
    constexpr theQueue() = default;
    template<typename... Args, typename = 
    typename std::enable_if<(... && std::is_constructible_v<T,Args>) >::type > 
    explicit constexpr theQueue(Args&&... theList) : commonQueueFunctions<T, N, Idxtype>(0, sizeof...(theList),
    sizeof...(theList),std::forward<Args>(theList)...) {}

    constexpr bool push(T theObj){
        if(this->theSize == N){
            return false;//queue is full
        }
        new(&this->theArray[(this->tail == N ? (this->tail = 0)++ : this->tail++)].value)T(std::move(theObj));
        return ++this->theSize; //++theSize always > 0. Return true
    }
    template<typename ...Args> 
    constexpr bool emplace(Args&&... args){
        if(this->theSize == N){
            return false;//queue is full
        }
        new(&this->theArray[(this->tail == N ? (this->tail = 0)++ : this->tail++)].value)T((args)...);
        return ++this->theSize;
    }
    constexpr bool pop(){
        if(!this->theSize) return false; //If it's empty, pop fails
        this->theArray[(this->head == N ? this->head = 0 : this->head++)].value.~T();
        return this->theSize--;
    }
    ~theQueue(){ //Destroys every Cell's value where value is the active member
        this->clear();
    }
};
template<class T, std::size_t N, typename Idxtype = std::size_t>
using circularQueue = 
theQueue<T,N,std::is_trivially_destructible<T>::value && std::is_trivially_copy_assignable<T>::value, Idxtype>;
#endif //CIRCULARQUEUEHPP