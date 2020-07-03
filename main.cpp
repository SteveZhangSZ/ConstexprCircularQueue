#include <iostream>
#include <vector>
#include "circularQueue.hpp"
void testCQ(){
    std::cout << "Begin testCQ()\n";
    circularQueue<std::pair<short,short>, 3> thePairCQ(std::pair<short,short>{44,44}), secondPairCQ;
    thePairCQ.push({12,34}); 
    thePairCQ.emplace(98,10);
    thePairCQ.pop();
    circularQueue<std::pair<short,short>, 3> thirdPairCQ(thePairCQ);
    secondPairCQ = thePairCQ; //L value assignment with non trivial copy assignment
    secondPairCQ = std::move(thirdPairCQ); //R value assignment with non trivial copy assignment
    circularQueue<char, 3> theCharCQ('A','B');
    std::cout << std::boolalpha << theCharCQ.empty() << '\n';
    circularQueue<char, 3> secondCharCQ(theCharCQ); //copy by l value, trivially copyable type

    //copy by r value, trivially copyable type
    [[maybe_unused]] circularQueue<char, 3> thirdCharCQ(std::move(secondCharCQ)); 
    theCharCQ.push('C');
    circularQueue<std::string, 3> theStringCQ, secondStringCQ("Rhubarb");
    theStringCQ.push("Hello");
    theStringCQ.pop();
    theStringCQ = secondStringCQ; //L value assignment with non trivial copy assignment

    //copy by l value, non trivially copyable type
    circularQueue<std::string, 3> copyStringCQ(secondStringCQ),thirdStringCQ;
    thirdStringCQ = copyStringCQ;
    thirdStringCQ = circularQueue<std::string, 3>("Something here");
    [[maybe_unused]] const circularQueue<std::string, 3> emptyQueue;
    circularQueue<std::vector<int>, 2> theVecCQ;
    theVecCQ.emplace(4,4);
    std::cout << "theVecCQ.front().size() is " << theVecCQ.front().size() << '\n';
    theVecCQ.pop();
    theVecCQ.push({4,4});
    std::cout << "theVecCQ.front().size() is " << theVecCQ.front().size() << '\n';
}

void printCQ(){
    std::cout << "Begin printCQ\n";
    circularQueue<std::string, 3> theStringCQ("First");
    for(theStringCQ.push("Second"), theStringCQ.emplace("Third");
    !theStringCQ.empty(); theStringCQ.pop()){
        std::cout << theStringCQ.front() << '\n';
    }
    theStringCQ.emplace("Fourth");
    std::cout << theStringCQ.back() << '\n';
    
}

void testCopyCQ(){
    std::cout << "Begin testCopyCQ\n";
    struct printStruct{
        int theInt;
        printStruct() : theInt{-1} {std::cout << "printStruct's default ctor\n";}
        printStruct(int a) : theInt{a} {std::cout << "printStruct's int ctor\n";}
    };
    circularQueue<printStruct, 2> firstPrintCQ((printStruct(21)),(printStruct()));
    std::cout << "Begin the copy\n";
    circularQueue<printStruct, 2> copyOfPrintCQ(firstPrintCQ);
    circularQueue<printStruct, 2> secondCopyOfPrintCQ(std::move(copyOfPrintCQ));
}

void fullCQ(){
    std::cout << "Begin fullCQ\n";
    circularQueue<char, 2> theFullCQ, anotherCQ;
    theFullCQ.push('A');
    theFullCQ.push('B');
    theFullCQ.push('C');
    theFullCQ.pop();
    theFullCQ.push('C');
    theFullCQ.push('D');
    anotherCQ = std::move(theFullCQ);
}
constexpr bool atCompileTimeCQ(){
    circularQueue<char, 3> theCharCQ('A','B'), fourthCQ;
    circularQueue<char, 3> secondCharCQ(theCharCQ); //copy by l value
    circularQueue<char, 3> thirdCharCQ(std::move(secondCharCQ)); //copy by r value
    theCharCQ.push('C');
    thirdCharCQ = circularQueue<char, 3>('X','Y'); //assignment via r value
    thirdCharCQ = theCharCQ; //assignment via l value
    thirdCharCQ.front() = 'D';
    fourthCQ.emplace('A');
    fourthCQ.pop();
    [[maybe_unused]] circularQueue<char, 3> fifthCQ(theCharCQ), sixthCQ(circularQueue<char, 3>('X','Y'));
    return true;
}

int main(){
    testCQ();
    printCQ();
    static_assert(atCompileTimeCQ());
    testCopyCQ();
    fullCQ();
    //atCompileTimeCQ();
}
//clang++ -Wall -std=c++1z -stdlib=libc++ -g main.cpp -o main && ./main