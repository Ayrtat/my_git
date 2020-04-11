//https://codeforces.com/problemset/problem/1334/B

#include <iostream>
#include <algorithm>
#include <map>
#include <vector>

int main() {
    int N;
    int lowerBound = 7;

    std::vector<int> savings{9, 4, 9};
    std::sort(savings.begin(), savings.end());

    auto maxSaving = std::max_element(savings.begin(), savings.end());

    if (*maxSaving == lowerBound)
    {
        N = std::count(savings.begin(), savings.end(), *maxSaving);
    }
    else if (*maxSaving < lowerBound)
    {
        N = 0;
    }
    else
    {
        auto it_of_max = std::find(savings.begin(), savings.end(), *maxSaving);

        auto amount_of_max = std::distance(it_of_max, savings.end());

        int sum = amount_of_max * *maxSaving;
        N = amount_of_max;

        auto lesser_savings = std::vector<int>(std::make_reverse_iterator(it_of_max), savings.rend());

        for (auto it = lesser_savings.begin(); it < lesser_savings.end(); it++)
        {
            sum += *it;
            ++N;

            float aportioned = float(sum) / N;

            if (aportioned < float(lowerBound))
            {
                --N;
                break;
            }
        }
    }

    std::cout << N;

    return 0;
}
