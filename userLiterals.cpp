#include <iostream>

double operator"" _some (long double v)
{
    std::cout << "Interpreted "
              << v
              << ";"
              << std::endl;
    return v;
}

double operator"" _some (unsigned long long int v)
{
    std::cout << "Interpreted "
              << v
              << ";"
              << std::endl;
    return v;
}

int main (int, char**)
{
    std::cout << 0xABC_some + 0.156_some << std::endl;
    return 0;
}
