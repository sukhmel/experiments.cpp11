#include <algorithm>
#include <iterator>
#include <iostream>
#include <vector>
#include <deque>

template <typename In>
bool sort_1_2(In begin, In end)
{
    bool success = false;
    int ones = std::count(begin, end, 1),
        twos = std::count(begin, end, 2);
    if (ones + twos == std::distance(begin, end))
    {
        std::fill(begin, begin + ones, 1);
        std::fill(begin + ones, end, 2);
        success = true;
    }
    return success;
}

template <typename In>
void print(In begin, In end)
{
    std::copy(begin, end, 
              std::ostream_iterator<int>(std::cout, " "));
    std::cout << '\n';
}

template <typename In>
void sort_print(In begin, In end)
{
    std::cout << sort_1_2(begin,
                          end  )
              << '\n';
    print(begin, end);
    std::cout << "~~~~~~~~~~~~~~~~~~\n";
}

int main(int, char**)
{
    std::vector<int> v_ok = {1,2,2,1,1,1,2,2};
    std::vector<int> v_no = {1,2,1,2,2,0,1,1};
    
    std::deque<int> d_ok = {1,2,2,1,1,1,2,2};
    std::deque<int> d_no = {1,2,1,2,2,0,1,1};
    
    int a_ok [] = {1,2,2,1,1,1,2,2};
    int a_no [] = {1,2,1,2,2,0,1,1};
    
    print     ( v_ok.begin(), v_ok.end() );
    sort_print( v_ok.begin(), v_ok.end() );
    
    print     ( v_no.begin(), v_no.end() );
    sort_print( v_no.begin(), v_no.end() );
       
    print     ( d_ok.begin(), d_ok.end() );
    sort_print( d_ok.begin(), d_ok.end() );
    
    print     ( d_no.begin(), d_no.end() );
    sort_print( d_no.begin(), d_no.end() );
       
    print     ( &a_ok[0], &a_ok[8] );
    sort_print( &a_ok[0], &a_ok[8] );
    
    print     ( &a_no[0], &a_no[8] );
    sort_print( &a_no[0], &a_no[8] );

    return 0;
}
