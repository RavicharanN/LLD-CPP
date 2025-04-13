#include <bits/stdc++.h>
using namespace std;

// funcPtr are of the type : " return_type (*ptr)(paramType1, paramType2...) = procedure_name"

void add(int a, int b, int &c)
{
    c = a + b;
}

int main()
{
    int a = 0, b = 1, c = 4;
    void (*funcPtr)(int, int, int &) = add;
    funcPtr(a, b, c);
    cout << c;
}