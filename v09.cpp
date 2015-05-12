#include "fluff.hpp"
#include <type_traits>

namespace detail
{
  template <typename Fn>
  class DefaultCase
  {
  public:
    explicit DefaultCase(Fn fn) : fn(fn) {}

    bool supports(const std::type_info&) const { return false; }
    template <typename Any>
    auto call(Any&) { return fn(); }

  private:
    Fn fn;
  };

  template <typename Fn>
  class EmptyCase
  {
  public:
    explicit EmptyCase(Fn fn) : fn(fn) {}

    bool supports(const std::type_info& t) const { return t == typeid(void); }
    template <typename Any>
    auto call(Any&) { return fn(); }

  private:
    Fn fn;
  };

  template <typename T, typename Fn>
  class TypeCase
  {
  public:
    explicit TypeCase(Fn fn) : fn(std::move(fn)) {}

    bool supports(const std::type_info& t) const { return t == typeid(T); }
    template <typename Any>
    auto call(Any& a) {
      return fn(*boost::unsafe_any_cast<T>(&a));
    }

  private:
    Fn fn;
  };

  template <typename Result, typename Any, typename Default>
  Result switchAnyImpl(Any& a, const std::type_info& t, Default d) {
    return d.call(a);
  }

  template <typename Result, typename Any, typename Default,
            typename Case, typename... Cases>
  Result switchAnyImpl(Any& a, const std::type_info& t, Default d,
                       Case c, Cases... cases) {
    if (c.supports(t))
      return c.call(a);
    return switchAnyImpl<Result>(a, t, d, cases...);
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

  template <typename Fn>
  struct FirstArgumentImpl
  {
    using type = typename FirstArgumentImpl<decltype(&Fn::operator())>::type;
  };

  template <typename R, typename Arg, typename... Args>
  struct FirstArgumentImpl<R (*)(Arg, Args...)>
  {
    using type = Arg;
  };
  template <typename R, typename Arg, typename... Args>
  struct FirstArgumentImpl<R(Arg, Args...)>
  {
    using type = Arg;
  };
  template <typename R, typename C, typename Arg, typename... Args>
  struct FirstArgumentImpl<R (C::*)(Arg, Args...)>
  {
    using type = Arg;
  };
  template <typename R, typename C, typename Arg, typename... Args>
  struct FirstArgumentImpl<R (C::*)(Arg, Args...) const>
  {
    using type = Arg;
  };

  template <typename Fn>
  using FirstArgument = typename FirstArgumentImpl<Fn>::type;

  template <typename Fn>
  using DeducedArgument = std::decay_t<FirstArgument<Fn>>;
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

template <typename Fn>
detail::TypeCase<detail::DeducedArgument<Fn>, Fn> typeCase_(Fn fn)
{
  return detail::TypeCase<detail::DeducedArgument<Fn>, Fn>(fn);
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

TEST(SwitchAny, canDeduceArgument)
{
	boost::any a = 3;
	int result = switchAny<int>(a,
		typeCase_([](int& i) { return i; }),
		emptyCase([] { return 2; }),
		defaultCase([] { return 1; })
	);
	EXPECT_EQ(3, result);
}

TEST(SwitchAny, canUseLambdaDirectlyForEmptyCase)
{
	boost::any a;
	int result = switchAny<int>(a,
		[] { return 2; },
		default_([] { return 1; })
	);
	EXPECT_EQ(2, result);
}
