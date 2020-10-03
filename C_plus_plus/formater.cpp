#include <iostream>
#include <algorithm>
#include <string>

void myPrintf(const char* format) {
	std::cout << format;
}

template<typename T, typename... Args>
void myPrintf(const char* format, T value, Args... args) {
	std::string format_as_string = std::string(format);
	auto flag_pos = format_as_string.find("%");

	std::cout << std::string(format, format + flag_pos) << value;

	myPrintf(format + flag_pos + 1, args...);
}

int main() {
	// the given line should print "Hello world! 2011"
	myPrintf("% world% %\n", "Hello", '!', 2011);
}
