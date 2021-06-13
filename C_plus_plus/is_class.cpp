#include <type_traits>
#include <iostream>
#include <string>

template<typename T, typename = void>
struct is_class : std::false_type {};

template<typename T>
struct is_class<T, std::void_t<int(T::*)>> : std::true_type {};

int main() {
	std::cout << is_class<std::string>::value;
	std::cout << is_class<int>::value;
}
