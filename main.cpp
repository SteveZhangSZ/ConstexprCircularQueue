#include <iostream> //Print statements for debugging
#include "circularQueue.hpp"
void testingCircularQueue(){
    circularQueue<std::pair<uint_least16_t,uint_least16_t>, 3> cQ(
            std::pair{59991,2}, std::pair{3,4} );

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
        std::cout <<  newQueue.push('a') << ' ' << newQueue.push('b') << ' ' << newQueue.push('c') << ' '
        << newQueue.pop() << ' ' << newQueue.push('c') << '\n';

        
}

int main(){
testingCircularQueue();
}
//clang++ -Wall -std=c++1z -stdlib=libc++ -g main.cpp -o main && ./main