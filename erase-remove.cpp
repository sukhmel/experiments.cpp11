#include <algorithm>
#include <iostream>
#include <iterator>
#include <vector>
#include <string>

// Using templates to determite stored data type for container
template<template <typename, typename> class C, typename V, typename T>
void out(C<V,T> &in)
{
    std::cout << '\n';
    std::copy( in.begin(),
               in.end(),
               std::ostream_iterator<V>(std::cout, " "));
}

// Using predefined ::value_type
template<typename Container>
void print(Container &in)
{
    std::copy( in.begin(),
               in.end(),
               std::ostream_iterator<typename Container::value_type>(std::cout, " "));
}

int main(int, char**)
{
    std::vector<std::string> test = {"a", "b", "a", "c", "c", "i"}; // C++11
    
    print(test);
    
    std::remove(test.begin(), test.end(), "a");
    out(test);  // Here values of last two elements are undefined
    
    std::random_shuffle(test.begin(), test.end());
    out(test);  // Values are mixed, and then
    
    test.erase(std::remove(test.begin(), test.end(), "a"), test.end());
    out(test);  // a's are deleted using erase-remove
    
    return 0;
}