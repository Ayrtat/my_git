#include <boost/config/warning_disable.hpp>
#include <boost/spirit/home/x3.hpp>
#include <boost/fusion/include/adapt_struct.hpp>
#include <boost/fusion/include/io.hpp>

#include <iostream>
#include <string>
#include <vector>
#include <optional>
#include <algorithm>
#include <numeric>

namespace my_parser {
	namespace natural
	{
		struct Sentence
		{
			std::string subject;
			std::string predicate;
		};

		using boost::fusion::operator<<;
	}
}

/*
 *	Fusion
 *	parsed
 *	(any_lexem_rule, any_lexem_rule) ->
 *	fusion::vector<std::vector<char>> ->
 *	fused: (subject, predicate)
 */

BOOST_FUSION_ADAPT_STRUCT(my_parser::natural::Sentence,
						  subject,
						  predicate)
	
namespace my_parser {
	namespace sentence
	{	
		namespace x3 = boost::spirit::x3;
		namespace ascii = x3::ascii;

		using x3::int_;
		using x3::lexeme;
		using ascii::char_;

		x3::rule<class Sentence, my_parser::natural::Sentence> const sentence = "sentence";
		
		auto const any_lexem_rule = lexeme[+(char_ - ' ')];

		auto const sentence_def = any_lexem_rule >> any_lexem_rule;

		/*
		 * sentence - rule. Used below
		 * sentence_def - mandatory because of BOOST_SPIRIT_DEFINE macros
		 */

		BOOST_SPIRIT_DEFINE(sentence);
	}


	template <typename ReturnT, typename IteratorT, typename RuleT>
	std::optional<ReturnT>
	getStructureIfParsed(IteratorT begin, IteratorT end, RuleT rule) {
		using namespace boost::spirit;

		ReturnT result;
		
		const bool is_phrase_parsed = x3::phrase_parse(begin, end, rule, x3::space, result);

		if (begin != end && !is_phrase_parsed)
		{
			return std::nullopt;
		}

		return std::make_optional(result);
	}
}

int main() {
	std::string input_sentence = "He laughs";
	
	const auto pars_result = my_parser::getStructureIfParsed<my_parser::natural::Sentence>(
		input_sentence.begin(), 
		input_sentence.end(), 
		my_parser::sentence::sentence);
	
	return 0;
}
