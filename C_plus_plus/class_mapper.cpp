// tempaltes.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#include <iostream>
#include <string>
#include <list>
#include <type_traits>

template<typename... Ts>
struct type_mapper;

template<>
struct type_mapper<>
{
	static void mapTo(...);
};

template<typename T, typename... Ts>
struct type_mapper<T, Ts...> : type_mapper<Ts...>
{
	static typename T::second_type mapTo(typename T::first_type);
	using type_mapper<Ts...>::mapTo;
};

template<typename T, typename MapperT>
struct GetTypeOnMap
{
	using type = decltype(MapperT::mapTo(std::declval<T>()));
};

template<typename T, typename MapperT>
using get_type_on_mapping = typename GetTypeOnMap<T, MapperT>::type;

int main(void)
{
	using mapper = type_mapper <  
		std::pair<int, double>,
		std::pair<double, int>,
		std::pair<float, std::string>>;

	using shouldBeDouble = get_type_on_mapping<int, mapper>;
	using shouldBeString = get_type_on_mapping<float, mapper>;
	
	std::cout << shouldBeDouble{2.9};
	std::cout << shouldBeString{"Hello"};

	return 0;
}
