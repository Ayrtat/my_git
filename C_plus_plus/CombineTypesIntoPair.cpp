#include <iostream>

template<>
struct TypeCombiner<Nil> {};

template<typename first_type, typename second_type>
struct CombineTypesIntoPairsImpl
{
	CombineTypesIntoPairsImpl()
	{
		std::cout << "Pair of types:\t" << typeid(first_type).name() << " and " << typeid(second_type).name() << std::endl;
	}
};

template<typename T1>
struct CombineTypesIntoPairsImpl<T1, Nil> {};

template<typename T, typename... Ts>
struct CombineTypesIntoPairs : CombineTypesIntoPairsImpl<Ts, T>...,
							   CombineTypesIntoPairsImpl<T, Ts>...,
							   CombineTypesIntoPairsImpl<T, T>,
							   CombineTypesIntoPairs<Ts...> {};

template<typename T>
struct CombineTypesIntoPairs<T> : CombineTypesIntoPairsImpl<T, T> {};


int main()
{
	CombineTypesIntoPairs<int, double, char, const char*> tc;
}
