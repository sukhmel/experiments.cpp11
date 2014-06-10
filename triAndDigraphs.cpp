??=include <iostream>
using namespace std;

??=define p(x) %:x
??=define s p(??!)
??=define n p(??/n)
??=define e p(\40)

%:define eq(a, b) a xor_eq a xor (b)
%:define k(a, b) ??=a s ??=b
%:define i(x) int x; cout << p(input) e ??=x p(:); cin >> x;

int main(int,char**)??<
i(first)
i(second)
i(third)
cout << k(first, second) << n;
third xor_eq first not_eq second ?
             first bitor  second :
             first bitand second ;

cout << third << n;
eq(third, first + second);
cout << third << n;

int balu = 0xBALU;
cout << balu << n;

int a = 'one';
cout << a << n ;

return 0;??>
