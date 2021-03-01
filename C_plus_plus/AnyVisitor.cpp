#include <tuple>
#include <map>
#include <typeindex>
#include <typeinfo>
#include <iostream>
#include <any>
#include <functional>

struct Nil
{
	using head = nullptr_t;
};

template<typename HeadT, typename TailT = Nil>
struct cons
{
	using head = HeadT;
	using tail = TailT;
};

template<typename T, typename... Ts>
struct type_list
{
	using head = T;
	using tail = type_list<Ts...>;
};

template<typename T>
struct type_list<T>
{
	using head = T;
	using tail = Nil;
};

enum class Operators { plus, minus, divides, multiplies };

std::map<std::tuple<std::type_index, std::type_index, Operators>,
	std::function<std::any(std::any const&, std::any const&)>>
	global_any_visitor{};

template<typename> struct TypeCombiner;

#define DEFINE_FUNCTIONAL_OBJECT(NAMESPACE, FUNCTOR_NAME)					\
	template<typename  LeftOperandT, typename  RightOperandT>				\
    std::function<std::any(std::any const&, std::any const&)>				\
set_functional_object_##FUNCTOR_NAME()										\
{																			\
	return [](std::any const& lhs, std::any const& rhs) -> std::any         \
	{																		\
		const auto original_lhs = std::any_cast<const LeftOperandT&>(lhs);  \
		const auto original_rhs = std::any_cast<const RightOperandT&>(rhs); \
		using common_type = std::common_type_t<LeftOperandT, RightOperandT>;\
		const auto functor = NAMESPACE::FUNCTOR_NAME<common_type>{};		\
		return functor(original_rhs, original_rhs);						    \
	};																		\
}

DEFINE_FUNCTIONAL_OBJECT(std, plus)
DEFINE_FUNCTIONAL_OBJECT(std, minus)
DEFINE_FUNCTIONAL_OBJECT(std, multiplies)
DEFINE_FUNCTIONAL_OBJECT(std, divides)

#define ADD_ONE_PAIR_TO_VISITOR(visitor_name, first_type, second_type, functor_name) \
	visitor_name[std::make_tuple(std::type_index(typeid(first_type)), std::type_index(typeid(second_type)), Operators::##functor_name)] = \
				set_functional_object_##functor_name<first_type, second_type> ();

#define ADD_TO_VISITOR(visitor_name, first_type, second_type, functor_name)     \
	ADD_ONE_PAIR_TO_VISITOR(visitor_name, first_type, first_type, functor_name) \
	ADD_ONE_PAIR_TO_VISITOR(visitor_name, first_type, second_type, functor_name) \
	ADD_ONE_PAIR_TO_VISITOR(visitor_name, second_type, second_type, functor_name) \
	ADD_ONE_PAIR_TO_VISITOR(visitor_name, second_type, first_type, functor_name) \

template<typename T1, typename TypeList>
struct TypeCombinerImpl : virtual TypeCombinerImpl<T1, typename TypeList::tail>
{
	using first_type = T1;
	using second_type = typename TypeList::head;

	TypeCombinerImpl()
	{
		ADD_TO_VISITOR(global_any_visitor, first_type, second_type, plus)
		ADD_TO_VISITOR(global_any_visitor, first_type, second_type, minus)
		ADD_TO_VISITOR(global_any_visitor, first_type, second_type, divides)
		ADD_TO_VISITOR(global_any_visitor, first_type, second_type, multiplies)

		std::cout << "TypeCombinerImpl\t" << typeid(first_type).name() << " and " << typeid(second_type).name() << std::endl;
	}
};

template<typename T1>
struct TypeCombinerImpl<T1, Nil> {};


template<typename TypeListT>
struct TypeCombiner : TypeCombinerImpl<typename TypeListT::head, TypeListT>, TypeCombiner<typename TypeListT::tail> {};

template<>
struct TypeCombiner<Nil> {};


int main()
{
	using tuple = type_list<int, double, char, float, long>;

	TypeCombiner<tuple> c;
	
	std::any first = 5;
	std::any second = 5.1112;

	auto functor = global_any_visitor[std::make_tuple(std::type_index(first.type()), std::type_index(second.type()), Operators::plus)];

	auto res = functor(first, second);
	
	return 0;
}
