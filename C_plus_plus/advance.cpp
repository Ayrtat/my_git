// TemplatesTagDispatching.cpp

#include <iterator>
#include <forward_list>
#include <list>
#include <vector>
#include <iostream>

template <typename InputIterator, typename Distance>
void advance_impl(InputIterator& i, Distance n, std::input_iterator_tag) {
    std::cout << "InputIterator used" << std::endl;
    while (n--) ++i;
}

template <typename BidirectionalIterator, typename Distance>
void advance_impl(BidirectionalIterator& i, Distance n, std::bidirectional_iterator_tag) {
    std::cout << "BidirectionalIterator used" << std::endl;
    if (n >= 0)
        while (n--) ++i;
    else
        while (n++) --i;
}

template <typename RandomAccessIterator, typename Distance>
void advance_impl(RandomAccessIterator& i, Distance n, std::random_access_iterator_tag) {
    std::cout << "RandomAccessIterator used" << std::endl;
    i += n;
}

template <typename InputIterator, typename Distance>
void advance_(InputIterator& i, Distance n) {
    typename std::iterator_traits<InputIterator>::iterator_category category;
    advance_impl(i, n, category);
}

int main() {
    // call the above functions to move them 5 position further
    std::vector<int> v = { 1, 2, 3, 4, 5 };
    std::forward_list<int> fw = { 1, 2, 3, 4, 5 };
    std::list<int> l = { 1, 2, 3, 4, 5 };

    auto it_v = v.begin();
    auto it_f = fw.begin();
    auto it_l = l.begin();

    advance_(it_v, 1);
    advance_(it_f, 1);
    advance_(it_l, 1);
}
