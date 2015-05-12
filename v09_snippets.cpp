template <typename Fn>
detail::DefaultCase<Fn> default_(Fn fn)
{
  return detail::DefaultCase<Fn>(fn);
}

detail::asCase(cases)

  template <typename C>
  void asCase(C c);

  template <typename T, typename Fn>
  TypeCase<T, Fn> asCase(TypeCase<T, Fn> c) {
    return c;
  }

  template <typename Fn>
  EmptyCase<Fn> asCase(EmptyCase<Fn> c) {
    return c;
  }

  template <typename Fn>
  DefaultCase<Fn> asCase(DefaultCase<Fn> c) {
    return c;
  }

  template <typename C>
  auto asCase(C c)
    -> std::enable_if_t<IsCallable<C()>::value, EmptyCase<C>>
  {
    return EmptyCase<C>(c);
  }

  template <typename Fn, typename... Args>
  std::true_type IsCallableHelper(decltype(std::declval<Fn>()(std::declval<Args>()...))*);
  template <typename Fn, typename... Args>
  std::false_type IsCallableHelper(...);

  template <typename Call>
  struct IsCallable { static const bool value = false; };
  template <typename Fn, typename... Args>
  struct IsCallable<Fn(Args...)>
    : decltype(IsCallableHelper<Fn, Args...>(0))
  {};
