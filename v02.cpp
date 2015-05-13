#include "fluff.hpp"

namespace detail
{
  template <typename Fn>
  class DefaultCase
  {
  public:
    explicit DefaultCase(Fn fn) : fn(fn) {}

    Fn getFn() const { return fn; }

  private:
    Fn fn;
  };

  template <typename Result>
  Result switchAnyImpl(boost::any& a) {
    assert(false && "No case matched.");
    std::terminate();
  }

  template <typename Result, typename Case, typename... Cases>
  Result switchAnyImpl(boost::any& a, Case c, Cases... cases) {
    return c.getFn()();
  }
}

template <typename Result, typename... Cases>
Result switchAny(boost::any& a, Cases... cases)
{
  return detail::switchAnyImpl<Result>(a, cases...);
}

template <typename Fn>
detail::DefaultCase<Fn> defaultCase(Fn fn)
{
  return detail::DefaultCase<Fn>(fn);
}



TEST(SwitchAny, usesDefaultCase)
{
	boost::any a;
	int result = switchAny<int>(a,
		defaultCase([] { return 1; })
	);
	EXPECT_EQ(1, result);
}

TEST(SwitchAny, usesEmptyCaseForEmpty)
{
	boost::any a;
	int result = switchAny<int>(a,
		emptyCase([] { return 2; }),
		defaultCase([] { return 1; })
	);
	EXPECT_EQ(2, result);
}
