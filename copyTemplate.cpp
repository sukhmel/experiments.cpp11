#include <iostream>

template <typename In, typename Out>
void copy (In begin, In end, Out destination)
{
    while (begin != end)
    {
        *destination++ = *begin++;
    }
}

template <typename In, typename Out>
void copy (In begin, int size, Out destination)
{
    copy(begin, begin + size, destination);
}

int main(int, char**)
{
    int a [10] = {1,2,3,4,5,6,7,8};
    int b [12] = {0}; // implicit 0s
    
    copy(&a[0], 10, &b[0]);
    
    for (int index = 0; index < 12; ++index)
    {
        std::cout << b[index] << std::endl;
    }
    
    return 0;
}