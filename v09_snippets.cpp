// Shorter name is prettier when the other cases have no wrapper at all.
template <typename Fn>
detail::DefaultCase<Fn> default_(Fn fn)
{
  return detail::DefaultCase<Fn>(fn);
}

// Transform each case to a case object.
detail::asCase(cases)

  // Leave the base version unimplemented for now.
  template <typename C>
  void asCase(C c);

  // Overloads that leave the existing case classes untouched.
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

  // We want to wrap things that are callable without arguments (such as
  // lambdas without arguments) in an EmptyCase.
  template <typename C>
  auto asCase(C c)
    -> std::enable_if_t<IsCallable<C()>::value, EmptyCase<C>>
  {
    return EmptyCase<C>(c);
  }

  // Incorrect usage. Should always be used as IsCallable<Fn(Args)>
  template <typename Call>
  struct IsCallable { static const bool value = false; };

  // Separate function and arguments. Derive from what the chosen overload
  // of the helper returns.
  template <typename Fn, typename... Args>
  struct IsCallable<Fn(Args...)>
    : decltype(IsCallableHelper<Fn, Args...>(0))
  {};

  // Extended SFINAE: this overload is chosen if the expression within the
  // decltype is well-formed.
  template <typename Fn, typename... Args>
  std::true_type IsCallableHelper(decltype(std::declval<Fn>()(std::declval<Args>()...))*);
  // This overload is chosen as a last resort.
  template <typename Fn, typename... Args>
  std::false_type IsCallableHelper(...);
