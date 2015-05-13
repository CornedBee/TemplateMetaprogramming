#include <boost/any.hpp>
#include <iostream>
#include <string>

using namespace boost;
using namespace std::literals;

int main() {
  std::cout << std::boolalpha;

  // Can store value of any (copyable) type.
  any a = 42;

  // Use any_cast to get the value out. You have to put the exact type there.
  std::cout << any_cast<int>(a) << "\n";

  // Casting an any* returns a pointer, which is null if the type is wrong.
  long* aslong = any_cast<long>(&a);
  std::cout << (aslong == nullptr) << "\n";

  try {
    // Type mismatch for a reference/value cast throws.
    any_cast<bool>(a);
  } catch (bad_any_cast& e) {
    std::cout << e.what() << "\n";
  }

  // The type() function gets the type_info object for the contained type.
  a = "hello"s;
  std::cout << a.type().name() << "\n";

  // any is copyable.
  any a2 = a;
  std::cout << any_cast<std::string>(a2) << "\n";

  // The underlying object is really copied.
  // You can cast to reference to get a reference to the contained object.
  auto& s1 = any_cast<std::string&>(a);
  s1 = "bye";
  std::cout << any_cast<std::string>(a) << "\n";
  std::cout << any_cast<std::string>(a2) << "\n";
}
