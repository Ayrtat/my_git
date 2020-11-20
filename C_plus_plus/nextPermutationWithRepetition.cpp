#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <chrono>
#include <numeric>
#include <regex>
#include <list>
#include <map>

template<typename InputIt>
bool nextPermutationWithRepetition(InputIt begin,
                                    InputIt end,
                                    typename InputIt::value_type from_value,
                                    typename InputIt::value_type to_value) {
    auto stop_iteration = std::all_of(begin, end, [&to_value](auto value) { return (value == to_value); });

    if(stop_iteration)
        return  false;

    typename InputIt::value_type search_this_value[] = { to_value };

    auto bound_element_iterator = std::find_end(begin,
                                                end,
                                                std::begin(search_this_value),
                                                std::end(search_this_value),
                                                [](auto left, auto right) { return left != right; });

    (*bound_element_iterator)++;
    std::fill(std::next(bound_element_iterator), end, from_value);

    return true;
}

int main() {
    std::list<int> vec(3, 0);

    do {
        std::copy(vec.begin(), vec.end(), std::ostream_iterator<int>(std::cout, " "));
        std::cout << std::endl;
    } while (nextPermutationWithRepetition(vec.begin(), vec.end(), 0, 2));

    return  0;
}
