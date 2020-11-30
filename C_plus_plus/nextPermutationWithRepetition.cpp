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

template<typename InputIt, typename ContainerT>
bool nextPermutationWithRepetition_v(InputIt begin, InputIt end, ContainerT from_value, ContainerT to_value) {
    if(std::distance(begin, end) != std::size(from_value))
        return false;
    if(std::distance(begin, end) != std::size(to_value))
        return false;

    const auto pairOfIt = std::mismatch(std::make_reverse_iterator(end),
                                        std::make_reverse_iterator(begin),
                                        std::make_reverse_iterator(std::end(to_value)));

    auto it = pairOfIt.first;

    if (it == std::make_reverse_iterator(begin))
        return false;

    auto bound_element_iterator = std::prev(it.base());
    auto dist = std::distance(begin, bound_element_iterator);

    (*bound_element_iterator)++;
    std::copy(std::begin(from_value) + dist, std::end(from_value), std::next(bound_element_iterator));

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
