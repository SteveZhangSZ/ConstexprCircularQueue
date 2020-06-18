#include <iostream> //Print statements for debugging
#include "circularQueue.hpp"

void testingCircularQueue(){
    circularQueue<std::pair<uint_least16_t,uint_least16_t>, 3> cQ(std::pair{59991,2}/*, std::pair{3,4}*/ );
    std::cout << std::boolalpha << cQ.push({3,3}) << ' ' << cQ.push({13,13}) << ' '
    << cQ.push({32,23}) << ' ' << cQ.push({321,231}) << ' ' << cQ.push({321,231})
    << ' ' << cQ.push({321,231}) << '\n';

    cQ.pop();
    std::cout << cQ.push({3,3}) << ' ' << cQ.push({321,231}) << '\n';
    std::cout << "Begin popping\n";
    std::cout << cQ.pop() << ' ' << cQ.pop() << ' ' << cQ.pop() << ' ' << cQ.pop() << '\n';

    std::cout << "Initially empty queue\n";
    circularQueue<char, 5> charCq;
    std::cout << charCq.pop() << '\n';
    charCq.push('D');
    std::cout << charCq.front() << ' ' << charCq.pop() << '\n';

    circularQueue<char, 2> newQueue;
    std::cout <<  newQueue.push('a') << ' ' 
    << newQueue.push('b') << ' ' 
    << newQueue.push('c') << ' '
    << newQueue.pop() << ' ' << newQueue.push('Z') << '\n';

    const circularQueue<char, 2> copy(newQueue); //test copy with non const obj
    const circularQueue<char, 2> copyOfcopy(copy);//test copy with const obj
    [[maybe_unused]] constexpr circularQueue<char, 2> fromTemp((circularQueue<char, 2>())); //from temp
    circularQueue<char, 2> testAssignment = circularQueue<char, 2>();//assignment with r value
    testAssignment = copy; //assignment with const copy
    testAssignment = newQueue;//assignment with non const copy
    constexpr circularQueue<char, 2, uint_least8_t> notAnError('A', 'Z');
    [[maybe_unused]] constexpr circularQueue<char, 2, uint_least8_t> copyFromConstexpr(notAnError);
    std::cout << sizeof(copy) << ' ' << sizeof(notAnError) << '\n';
    struct A{};
    //[[maybe_unused]] constexpr circularQueue<char, 2, A> errorThisis;

    
    for(circularQueue<char, 5, uint_least8_t> testingFrontAndPop{'A', 'B', 'C', 'D', 'E'}; 
    !testingFrontAndPop.empty(); testingFrontAndPop.pop()){
        std::cout << testingFrontAndPop.front() << ' ';
    }
    std::cout << '\n';
}
constexpr bool otherTest(){
    circularQueue<char, 2> newQueue;
    constexpr char anA{'a'};
    newQueue.push(anA);
    newQueue.push('b');
    newQueue.push('c');
    newQueue.pop();
    newQueue.emplace('d');
    newQueue.pop();
    return newQueue.emplace('e');
}

int main(){
testingCircularQueue();
if constexpr(otherTest()) std::cout << "If constexpr succeeded\n";
//std::cout << std::is_integral<std::string>::value << '\n';
//std::cout << std::boolalpha << std::is_constructible<std::pair<uint_least16_t,uint_least16_t>,std::pair<uint_least16_t,uint_least16_t>>::value << '\n';
}
//clang++ -Wall -std=c++1z -stdlib=libc++ -g main.cpp -o main && ./main