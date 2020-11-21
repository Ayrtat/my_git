#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <chrono>
#include <numeric>
#include <regex>
#include <list>
#include <map>

template<typename InputIt, typename T>
bool nextPermutationWithRepetition(InputIt begin, InputIt end, T from_value, T to_value) {
	auto it = std::find_if_not(std::make_reverse_iterator(end),
							   std::make_reverse_iterator(begin),
							   [&to_value](auto current) { return to_value == current; });

	if (it == std::make_reverse_iterator(begin))
		return false;

	auto bound_element_iterator = std::prev(it.base());

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
