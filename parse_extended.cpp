#include <array>
#include <iomanip>
#include <iostream>
#include <set>
#include <string>
#include <type_traits>

#include <tao/pegtl.hpp>
#include <tao/pegtl/contrib/parse_tree.hpp>
#include <tao/pegtl/contrib/parse_tree_to_dot.hpp>

using namespace TAO_PEGTL_NAMESPACE;

namespace example
{
    // the grammar

    // clang-format off

    struct integer : tao::pegtl::plus<digit> {};
    struct variable : identifier {};

    struct TradeSumm : TAO_PEGTL_STRING("Сделка:Сумма") {};
    struct TradePrice : TAO_PEGTL_STRING("Сделка:Цена") {};
    struct InstrPriceOfPreviousTrade : TAO_PEGTL_STRING("Инстр:ЦенаПредпослСделки") {};

    struct PrefixAndMetric : sor<TradeSumm, TradePrice, InstrPriceOfPreviousTrade> {};

    struct ModuleFunctionStringName : TAO_PEGTL_STRING("МОДУЛЬ") {};
    struct ToStringFunctionStringName : TAO_PEGTL_STRING("СТРОКА") {};
    struct AsStrFunctionStringName : TAO_PEGTL_STRING("СТР") {};

    struct functionWithOneParameter : sor<ModuleFunctionStringName, ToStringFunctionStringName> {};
    struct functionWithTwoParameters : sor<AsStrFunctionStringName> {};

    struct StringObject : seq<star< space >, one<'"'>, seq<tao::pegtl::plus<alnum> >, one<'"'>, star< space > > {};
    struct IdenteficatorObject : seq<tao::pegtl::plus<alnum> > {};

    struct plus : pad< one< '+' >, space > {};
    struct minus : pad< one< '-' >, space > {};
    struct multiply : pad< one< '*' >, space > {};
    struct divide : pad< one< '/' >, space > {};

    struct equal : seq<star< space >, tao::pegtl::string< '=', '=' >, star< space > > {};
    struct notEqual : seq<star< space >, tao::pegtl::string< '<', '>' >, star< space > > {};
    struct lessOrEqual : seq<star< space >, tao::pegtl::string< '<', '=' >, star< space > > {};
    struct moreOrEqual : seq<star< space >, tao::pegtl::string< '>', '=' >, star< space > > {};
    struct less : seq<star< space >, one<'<'>, not_one<'='>, star< space > > {};
    struct more : seq<star< space >, one<'>'>, not_one<'='>, star< space > > {};
    struct comparassion : sor<less, more, lessOrEqual, moreOrEqual, notEqual, equal> {};

    struct open_bracket : seq< one< '(' >, star< space > > {};
    struct close_bracket : seq< star< space >, one< ')' > > {};

    struct expression;
    struct bracketed : if_must< open_bracket, expression, close_bracket > {};

    struct functionCallWithOneParameter : if_must<functionWithOneParameter, bracketed> {};
    struct functionCallWithTwoParameter : if_must<functionWithTwoParameters,
        seq<open_bracket, expression, star<space>, one<','>, star<space>, expression, close_bracket> > {};


    struct functionCall : sor<functionCallWithOneParameter, functionCallWithTwoParameter> {};
    struct value : sor< integer, variable, bracketed, functionCall, PrefixAndMetric, StringObject> {};
    struct product : list_must< value, sor< multiply, divide > > {};
    struct expression : list_must< product, sor< plus, minus, comparassion >> {};

    struct statement;
    struct assignment : seq<star<space>,
        variable, star<space>,
        tao::pegtl::string <':', '='>, star<space>, expression, star<space> > {};


    struct braced_statements : seq<star<space>, one<'{'>, star<space>, star<statement>, star<space>, one<'}'>, star<space> > {};
    struct ifElseStatement : seq<star< space >,
        tao::pegtl::string<'i', 'f'>, star< space >, bracketed,
        star< space >, sor<statement,braced_statements>, star< space >,
        tao::pegtl::string<'e', 'l', 's', 'e'>,
        star< space >, sor<statement,braced_statements>, star< space > > {};
    struct statement : sor<seq<assignment, one<';'> >, seq<expression, one<';'>,star<space> >, ifElseStatement > {};

    struct grammar : must< star<statement>, eof > {};
    // clang-format on

    std::set< std::string > identificators;

    struct PutInSymbolTable : parse_tree::apply< PutInSymbolTable >
    {
        template< typename Node, typename... States >
        static void transform(std::unique_ptr< Node >& n, States&&... st)
        {
            /*Put identificator in symbol type*/
            auto& c = n->children;
            n->remove_content();
            identificators.insert(c.front()->string());
        }
    };

    // after a node is stored successfully, you can add an optional transformer like this:
    struct rearrange
        : parse_tree::apply< rearrange >  // allows bulk selection, see selector<...>
    {
        // recursively rearrange nodes. the basic principle is:
        //
        // from:          PROD/EXPR
        //                /   |   \          (LHS... may be one or more children, followed by OP,)
        //             LHS... OP   RHS       (which is one operator, and RHS, which is a single child)
        //
        // to:               OP
        //                  /  \             (OP now has two children, the original PROD/EXPR and RHS)
        //         PROD/EXPR    RHS          (Note that PROD/EXPR has two fewer children now)
        //             |
        //            LHS...
        //
        // if only one child is left for LHS..., replace the PROD/EXPR with the child directly.
        // otherwise, perform the above transformation, then apply it recursively until LHS...
        // becomes a single child, which then replaces the parent node and the recursion ends.
        template< typename Node, typename... States >
        static void transform(std::unique_ptr< Node >& n, States&&... st)
        {
            if (n->children.size() == 1) {
                n = std::move(n->children.back());
            }
            else {
                n->remove_content();
                auto& c = n->children;
                auto r = std::move(c.back());
                c.pop_back();
                auto o = std::move(c.back());
                c.pop_back();
                o->children.emplace_back(std::move(n));
                o->children.emplace_back(std::move(r));
                n = std::move(o);
                transform(n->children.front(), st...);
            }
        }
    };

    struct rearrangeFunctionCall : parse_tree::apply< rearrangeFunctionCall >
    {
        template< typename Node, typename... States >
        static void transform(std::unique_ptr< Node >& n, States&&... st)
        {
            /*
            Own custom.
            I don't want functionCall node to be showed in parsing tree because instead
            I can put there a function name and from this function name there can be ramification on parameters

            */
            if (n->children.size() == 1) {
                n = std::move(n->children.back());
            }
            else {
                n->remove_content();
                auto& c = n->children;

                auto newNode = std::move(c.front());
                while (n->children.size() > 1) {
                    newNode->children.push_back(std::move(c.back()));
                    c.pop_back();
                }

                n = std::move(newNode);
            }
        }
    };

    // select which rules in the grammar will produce parse tree nodes:

    template< typename Rule >
    using selector = parse_tree::selector <
        Rule,
        parse_tree::store_content::on<
        StringObject,
        //ifElseStatement,
        integer,
        variable >,
        parse_tree::remove_content::on<
        plus,
        minus,
        multiply,
        //    assignment,
        divide,
        // functionCall,
        TradeSumm,
        TradePrice,
        InstrPriceOfPreviousTrade,
        ToStringFunctionStringName,
        ModuleFunctionStringName,
        AsStrFunctionStringName,
        // PrefixAndMetric,
        less,
        more,
        lessOrEqual,
        moreOrEqual,
        notEqual, 
        equal,
        braced_statements,
        ifElseStatement >, 
        PutInSymbolTable::on<assignment>,
        rearrangeFunctionCall::on< functionCall >,
        rearrange::on<  //functionCall,
        product,
        //  ifElseStatement,
        expression > > ;

    namespace internal
    {
        // a non-thread-safe allocation cache, assuming that the size (sz) is always the same!
        template< std::size_t N >
        struct cache
        {
            std::size_t pos = 0;
            std::array< void*, N > data;

            cache() = default;

            cache(const cache&) = delete;
            cache(cache&&) = delete;

            ~cache()
            {
                while (pos != 0) {
                    ::operator delete(data[--pos]);
                }
            }

            cache& operator=(const cache&) = delete;
            cache& operator=(cache&&) = delete;

            void* get(std::size_t sz)
            {
                if (pos != 0) {
                    return data[--pos];
                }
                return ::operator new(sz);
            }

            void put(void* p)
            {
                if (pos < N) {
                    data[pos++] = p;
                }
                else {
                    ::operator delete(p);
                }
            }
        };

        static cache< 32 > the_cache;

    }  // namespace internal

    // this is not necessary for the example, but serves as a demonstration for an additional optimization.
    struct node
        : parse_tree::basic_node< node >
    {
        void* operator new(std::size_t sz)
        {
            assert(sz == sizeof(node));
            return internal::the_cache.get(sz);
        }

        void operator delete(void* p)
        {
            internal::the_cache.put(p);
        }
    };

}  // namespace example

int main()
{
    setlocale(LC_ALL, "RUS");
   char* argv1[] = {
      //"СТР(МОДУЛЬ(Сделка:Цена), 4) + (\"Comment\"+(МОДУЛЬ(Сделка:Цена - Инстр:ЦенаПредпослСделки) / Инстр:ЦенаПредпослСделки * 100 >= 0) > (Сделка:Сумма >= 0))"
      /*"if(Сделка:Цена > 0) \
          if(Сделка:Цена > 0) \
             FirstVar := МОДУЛЬ(Сделка:Цена - Инстр:ЦенаПредпослСделки); \
             SecondVar := МОДУЛЬ(Сделка:Цена / Инстр:ЦенаПредпослСделки); \
             FirstVar := FirstVar + SecondVar; \
          else \
             Инстр:ЦенаПредпослСделки * 100;\
       else \
          Инстр:ЦенаПредпослСделки * 100;"*/
      "FirstVar := МОДУЛЬ(Сделка:Цена - Инстр:ЦенаПредпослСделки); \
       SecondVar := МОДУЛЬ(Сделка:Цена / Инстр:ЦенаПредпослСделки); \
       FirstVar := FirstVar + SecondVar; \
       \
       if(Сделка:Цена > 0) { \
             FirstVar := МОДУЛЬ(Сделка:Цена - Инстр:ЦенаПредпослСделки); \
             SecondVar := МОДУЛЬ(Сделка:Цена / Инстр:ЦенаПредпослСделки); \
             FirstVar := FirstVar + SecondVar; \
       }\
        else \
       \
            Инстр:ЦенаПредпослСделки * 100;\
       "
        /* "if(Сделка:Цена > 0) \
            a := МОДУЛЬ(Сделка:Цена - Инстр:ЦенаПредпослСделки); \
         else \
            Инстр:ЦенаПредпослСделки * 100;"*/
            //  "a := МОДУЛЬ(Сделка:Цена - Инстр:ЦенаПредпослСделки);"
    };
    argv_input in(argv1, 0);
    try {
        const auto root = parse_tree::parse< example::grammar, example::node, example::selector >(in);
        parse_tree::print_dot(std::cout, *root);
        std::copy( example::identificators.begin(), example::identificators.end(), std::ostream_iterator< std::string >( std::cout, "\n" ) );
        return 0;
    }
    catch (const parse_error& e) {
       const auto p = e.positions().front();
       std::cerr << e.what() << std::endl
                 << in.line_at( p ) << std::endl
                 << std::setw( p.column ) << '^' << std::endl;
    }
    catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
    }

    return 1;
}
