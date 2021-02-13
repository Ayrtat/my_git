// tempaltes.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#include <iostream>
#include <string>
#include <list>
#include <type_traits>

template<typename... Ts>
struct ClassMapper;

template<>
struct ClassMapper<>
{
	static void mapTo(...);
};

template<typename T, typename... Ts>
struct ClassMapper<T, Ts...> : ClassMapper<Ts...>
{
	static typename T::second_type mapTo(typename T::first_type);
	using ClassMapper<Ts...>::mapTo;
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
	using mapper = ClassMapper <  
		std::pair<int, double>,
		std::pair<double, int>,
		std::pair<float, std::string>>;

	using shouldBeDouble = get_type_on_mapping<int, mapper>;
	using shouldBeString = get_type_on_mapping<float, mapper>;
	
	std::cout << shouldBeDouble{2.9};
	std::cout << shouldBeString{"Hello"};

	return 0;
}
