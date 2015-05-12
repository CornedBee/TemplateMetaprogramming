#include "fluff.hpp"
#include <map>
#include <typeindex>
#include <type_traits>

namespace detail
{
  template <typename Fn>
  class DefaultCase
  {
  public:
    explicit DefaultCase(Fn fn) : fn(fn) {}

    bool supports(const std::type_info&) const { return false; }
    template <typename Result, typename Any>
    void registerIn(std::map<std::type_index, std::function<Result (Any&)>>&) {}
    auto call() { return fn(); }

  private:
    Fn fn;
  };

  template <typename Fn>
  class EmptyCase
  {
  public:
    explicit EmptyCase(Fn fn) : fn(fn) {}

    bool supports(const std::type_info& t) const { return t == typeid(void); }
    template <typename Result, typename Any>
    void registerIn(std::map<std::type_index, std::function<Result (Any&)>>&
                        cases) {
      cases.emplace(typeid(void), [this](Any& a) { return fn(); });
    }

  private:
    Fn fn;
  };

  template <typename T, typename Fn>
  class TypeCase
  {
  public:
    explicit TypeCase(Fn fn) : fn(std::move(fn)) {}

    bool supports(const std::type_info& t) const { return t == typeid(T); }
    template <typename Result, typename Any>
    void registerIn(std::map<std::type_index, std::function<Result (Any&)>>&
                        cases) {
      cases.emplace(typeid(T), [this](Any& a) {
        return fn(*boost::unsafe_any_cast<T>(&a));
      });
    }

  private:
    Fn fn;
  };

  template <typename Result, typename Any, typename Default, typename... Cases>
  Result switchAnyImpl(Any& a, const std::type_info& t, Default d,
                       Cases... cases) {
    using F = std::function<Result (Any&)>;
    std::map<std::type_index, F> caseLookup;

    (cases.registerIn(caseLookup), ...);
    /*int dummy[] = { 0,
      (cases.registerIn(caseLookup), 0)...
    };*/

    auto it = caseLookup.find(t);
    if (it == caseLookup.end())
      return d.call();
    return it->second(a);
  }

  template <typename Result>
  struct NoDefault
  {
    Result operator ()() {
      assert(false && "No case matched");
      std::terminate();
    }
  };
  template <>
  struct NoDefault<void>
  {
    void operator ()() const {}
  };

  template <typename Result>
  auto pickDefault() {
    return DefaultCase<NoDefault<Result>>(NoDefault<Result>());
  }

  template <typename Result, typename Case, typename... Cases>
  auto pickDefault(Case, Cases... cases)
  {
    return pickDefault<Result>(cases...);
  }

  template <typename Result, typename Fn, typename... Cases>
  auto pickDefault(DefaultCase<Fn> d, Cases...)
  {
    return d;
  }
}

template <typename Result = void, typename... Cases>
Result switchAny(boost::any& a, Cases... cases)
{
  return detail::switchAnyImpl<Result>(a, a.type(),
    detail::pickDefault<Result>(cases...), cases...);
}
template <typename Result = void, typename... Cases>
Result switchAny(const boost::any& a, Cases... cases)
{
  return detail::switchAnyImpl<Result>(a, a.type(),
    detail::pickDefault<Result>(cases...), cases...);
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

TEST(SwitchAny, worksForConst)
{
	boost::any a = 3;
	const boost::any& ra = a;
	int result = switchAny<int>(ra,
		typeCase<int>([](const int& i) { return i; }),
		emptyCase([] { return 2; }),
		defaultCase([] { return 1; })
	);
	EXPECT_EQ(3, result);
}

TEST(SwitchAny, orderIndependent)
{
	boost::any a = 3;
	int result = switchAny<int>(a,
		emptyCase([] { return 2; }),
		defaultCase([] { return 1; }),
		typeCase<int>([](const int& i) { return i; })
	);
	EXPECT_EQ(3, result);
}

TEST(SwitchAny, voidNeedsNoMatch)
{
	boost::any a = 3.0;
	int result = 1;
	switchAny(a,
		typeCase<int>([&](const int& i) { result = i; }),
		emptyCase([&] { result = 2; })
	);
	EXPECT_EQ(1, result);
}
