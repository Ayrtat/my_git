#include <iostream>
#include <vector>
#include <algorithm>
#include <numeric>
#include <list>
#include <set>
#include <iterator>

template<typename InputIt, typename T>
bool nextCombination(InputIt begin,
					 InputIt end,
					 T toElement) {
	/*
		Given sequence: 1 2 3 4 5
		Final sequence: 6 7 8 9 10

		-- Formally --
		Given sequence: 1 2 ... k-1 k
		Final sequence: (n-k+1) (n-k+2) ... (n-1) n

		lengthOfSubsequence = positionOf(5) - positionOf(1) = 5
		
		We look for an element that satisfies:
			seqeunce[pos] < n - k + pos

	*/

	const auto lengthOfSubsequence = std::distance(begin, end);
	auto pos = lengthOfSubsequence;

	auto viewed_element_it = std::make_reverse_iterator(end);
	auto reversed_begin = std::make_reverse_iterator(begin);

	/*Looking for this element here*/

	while ((viewed_element_it != reversed_begin) && (*viewed_element_it >= toElement -
													  lengthOfSubsequence + 
													  std::distance(viewed_element_it, reversed_begin))) {
		viewed_element_it++;
	}

	if (viewed_element_it == reversed_begin)
		return false;

	auto it = std::prev(viewed_element_it.base());

	/*
		Increment the found element. 
    The rest following elements we set as seqeunce[pos] = seqeunce[pos-1] + 1
	*/

	std::iota(it, end, *it + 1);

	return true;
}

int main()
{
	std::list<int> vec = { 1, 2, 3 };

	do {
		std::copy(vec.begin(), vec.end(), std::ostream_iterator<int>(std::cout, " "));
		std::cout << std::endl;
	} while (nextCombination(vec.begin(), vec.end(), 10));
}
