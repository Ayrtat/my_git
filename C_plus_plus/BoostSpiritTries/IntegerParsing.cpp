#include <boost/spirit/home/x3.hpp>

#include <iostream>
#include <string>
#include <vector>
#include <optional>
#include <numeric>

namespace parser {

	template <typename  IteratorT>
	std::optional<std::vector<int>>
	getStringsIfParsed(IteratorT begin, IteratorT end) {
		using namespace boost::spirit;

		std::vector<int> parsed_integers_vector;

		const auto& push_in = [&parsed_integers_vector](const auto& ctx)
		{
			const auto parsed_integer = x3::_attr(ctx);
			parsed_integers_vector.push_back(parsed_integer);
		};

		const auto& rule_for_parser = x3::int_[push_in] >> *(',' >> x3::int_[push_in]);
		const bool is_phrase_parsed = x3::phrase_parse(begin, end, rule_for_parser, x3::space);

		if(begin != end)
		{
			return std::nullopt;
		}

		if (!is_phrase_parsed)
		{
			return std::nullopt;
		}

		return std::make_optional(parsed_integers_vector);
	}
}

int main() {
	std::string input_string = "1, 4, 5, 6, 7";

	auto pars_result = parser::getStringsIfParsed(input_string.begin(), input_string.end());

	std::cout << "A sum of " << input_string << " is " << std::accumulate(pars_result->begin(), pars_result->end(), 0);

	return 0;
}
