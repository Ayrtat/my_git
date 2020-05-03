#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <algorithm>
#include <iterator>
#include <limits>
#include <numeric>
#include <map>

bool checkOnPrimaries(int num)
{
    std::vector<int> divisors = {2, 3, 5};
    while(num)
    {
        auto exists = false;
        auto divideOn = 1;
        
        for(auto divisor : divisors)
        {
            if(num % divisor == 0) 
            {
                exists = true;
                divideOn = divisor;
                break;
            }
        }
        
        if(exists) 
            num /= divideOn;
        else 
            break;
    }
    
    return num == 1 ? true : false;
}

int uglyNumber(int n)
{
    switch(n)
    {
        case 2:
            return 2;
        case 3:
            return 3;
        case 5:
            return 5;
    }
  
    int previousUgly = uglyNumber(n - 1);
    auto nextConvinient = previousUgly + 1;
    
    while(!checkOnPrimaries(nextConvinient))
    {
        nextConvinient++;
    }
    
    return nextConvinient;
}

int main()
{
    int number;
    std::cin >> number;
    
    std::cout << uglyNumber(number);
}
