#include <boost/any.hpp>
#include <iostream>
#include <string>

using namespace boost;
using namespace std::literals;

int main() {
  std::cout << std::boolalpha;
  any a = 42;
  std::cout << any_cast<int>(a) << "\n";
  long* aslong = any_cast<long>(&a);
  std::cout << (aslong == nullptr) << "\n";
  try {
    any_cast<bool>(a);
  } catch (bad_any_cast& e) {
    std::cout << e.what() << "\n";
  }

  a = "hello"s;
  std::cout << a.type().name() << "\n";

  any a2 = a;
  std::cout << any_cast<std::string>(a2) << "\n";

  auto& s1 = any_cast<std::string&>(a);
  s1 = "bye";
  std::cout << any_cast<std::string>(a) << "\n";
  std::cout << any_cast<std::string>(a2) << "\n";
}
