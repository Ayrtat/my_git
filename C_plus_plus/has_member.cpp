// tempaltes.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#include <iostream>
#include <string>
#include <list>
#include <type_traits>

#define LAMBDA_FOR_MEMBER_NAME(NAME) [](auto object_instance) -> decltype(&(decltype(object_instance)::NAME)) {}

template<typename T, typename LambdaExpressionT>
struct has_member {
    using lambda_prototype = LambdaExpressionT;

	//SFINAE
    template<class ValueT, class = void>
    struct is_void_t_deducable : std::false_type {};

    template<class ValueT>
    struct is_void_t_deducable<ValueT,
        std::void_t<decltype(std::declval<lambda_prototype>()(std::declval<ValueT>()))>> : std::true_type {};

    static constexpr bool value = is_void_t_deducable<T>::value;
};

struct Simple
{
	int field;
};

int main(void)
{
    auto lambda = LAMBDA_FOR_MEMBER_NAME(field);
    using lambda_type = decltype(lambda);

    std::cout << has_member<Simple, lambda_type>::value;
}
