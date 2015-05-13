#include <boost/any.hpp>
#include <iostream>
#include <string>
using namespace std::literals;

template <typename T>
T* decay(T* t) { return t; }
template <typename T>
const T* decay(const T* t) { return t; }


void problem()
{
	boost::any a1 = "yes"s;
	// Assigning from the literal directly doesn't compile.
	boost::any a2 = decay("no");
	std::string s1 = boost::any_cast<std::string>(a1);
	std::cout << s1 << "\n";
	std::string s2 = boost::any_cast<std::string>(a2);
	std::cout << s2 << "\n";
}















std::string stringFromAny(const boost::any& a);

void solution()
{
  // Contained type is const char*
	boost::any a1 = decay("yes1");
	std::string s1 = stringFromAny(a1);
	std::cout << s1 << "\n";

	// Contained type is std::string
	boost::any a2 = "yes2"s;
	std::string s2 = stringFromAny(a2);
	std::cout << s2 << "\n";

	// Contained type is char*
	char c3[] = "yes3";
	boost::any a3 = decay(c3);
	std::string s3 = stringFromAny(a3);
	std::cout << s3 << "\n";
}


std::string stringFromAny(const boost::any& a)
{
	if (auto* s = boost::any_cast<std::string>(&a)) {
		return *s;
	}
	if (auto* cp = boost::any_cast<const char*>(&a)) {
		return std::string(*cp);
	}
	if (auto* cp = boost::any_cast<char*>(&a)) {
		return std::string(*cp);
	}
	return std::string();
}













int main(int argc, const char** argv) {
  if (argc < 2 || argv[1] == "problem"s) problem();
  else solution();
}
