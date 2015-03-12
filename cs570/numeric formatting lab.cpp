/*******
 Sep 4, 2014
 *******/

#include <iostream>
using namespace std;

int main()
{
    long a;
    double b;
    cout << "Please enter a number" << endl;
    cin >> b;
    
    cout << "What you actual input "<<b<<endl;
    a = static_cast<size_t>(b);
    cout <<"The new integer variable "<< a << endl;
    
    return 0;
}