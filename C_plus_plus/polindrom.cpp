#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <algorithm>
#include <iterator>
#include <limits>
#include <numeric>
#include <map>

int begin = 0;
int end = 0;

std::string maxPolynrom;

int maxPolyndrom(std::string s)
{
    if (s.empty())
    {
        return 0;
    }

    if (s.size() == 1)
    {
        return 1;
    }

    if (s.front() == s.back())
    {
        std::string cutString;

        if (s.size() > 3)
            cutString = std::string(s.begin() + 1, s.end() - 1);
        else if (s.size() == 3)
            cutString = s[1];

        auto subpolyndrom = maxPolyndrom(cutString);
        if (subpolyndrom != 0 || cutString.empty())
        {
            if(maxPolynrom.size() < s.size())
                maxPolynrom = s;
            return 2 + subpolyndrom;
        }
        else
            return 0;
    }

    int max = 0;

    for (auto i = 0; i < s.length(); i++)
    {
        std::string onBack = std::string(s.begin(), s.end() - 1);
        std::string onFront = std::string(s.begin() + 1, s.end());

        max = std::max(max, std::max(maxPolyndrom(onFront), maxPolyndrom(onBack)));
    }

    return max;
}
int main()
{
    std::string inputString;
    std::cin >> inputString;

    maxPolyndrom(inputString);
    std::cout << maxPolynrom;

    return 0;
}
