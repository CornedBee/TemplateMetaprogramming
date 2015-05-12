#include "fluff.hpp"

namespace detail
{
  template <typename Fn>
  class DefaultCase
  {
  public:
    explicit DefaultCase(Fn fn) : fn(fn) {}

    bool supports(const std::type_info&) const { return true; }
    Fn getFn() const { return fn; }

  private:
    Fn fn;
  };

  template <typename Fn>
  class EmptyCase
  {
  public:
    explicit EmptyCase(Fn fn) : fn(fn) {}

    bool supports(const std::type_info& t) const { return t == typeid(void); }
    Fn getFn() const { return fn; }

  private:
    Fn fn;
  };

  template <typename T, typename Fn>
  class TypeCase
  {
  public:
    explicit TypeCase(Fn fn) : fn(std::move(fn)) {}

    bool supports(const std::type_info& t) const { return t == typeid(T); }
    Fn getFn() const { return fn; }

  private:
    Fn fn;
  };

  template <typename Result>
  Result switchAnyImpl(boost::any& a, const std::type_info& t) {
    assert(false && "No case matched.");
    std::terminate();
  }

  template <typename Result, typename Case, typename... Cases>
  Result switchAnyImpl(boost::any& a, const std::type_info& t,
                       Case c, Cases... cases) {
    if (c.supports(t))
      return c.getFn()();
    return switchAnyImpl<Result>(a, t, cases...);
  }
}

template <typename Result, typename... Cases>
Result switchAny(boost::any& a, Cases... cases)
{
  return detail::switchAnyImpl<Result>(a, a.type(), cases...);
}

template <typename Fn>
detail::DefaultCase<Fn> defaultCase(Fn fn)
{
  return detail::DefaultCase<Fn>(fn);
}

template <typename Fn>
detail::EmptyCase<Fn> emptyCase(Fn fn)
{
  return detail::EmptyCase<Fn>(fn);
}

template <typename T, typename Fn>
detail::TypeCase<T, Fn> typeCase(Fn fn)
{
  return detail::TypeCase<T, Fn>(fn);
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

TEST(SwitchAny, skipsEmptyCaseForNonEmpty)
{
	boost::any a = 1;
	int result = switchAny<int>(a,
		emptyCase([] { return 2; }),
		defaultCase([] { return 1; })
	);
	EXPECT_EQ(1, result);
}

TEST(SwitchAny, usesTypedCaseForFull)
{
	boost::any a = 3;
	int result = switchAny<int>(a,
		typeCase<int>([](int& i) { return i; }),
		emptyCase([] { return 2; }),
		defaultCase([] { return 1; })
	);
	EXPECT_EQ(3, result);
}
