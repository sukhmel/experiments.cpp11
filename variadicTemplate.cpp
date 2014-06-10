#include <iostream>

int countArgs()
{
    return 0;
}

template <typename T, typename ... Args>
int countArgs(T, Args ... args)
{
    return 1 + countArgs(args...);
}

int main(int, char**)
{
    std::cout << countArgs()
              << std::endl;
    std::cout << countArgs(1, 2, 3)
              << std::endl;
    std::cout << countArgs(0)
              << std::endl;
    return 0;
}
