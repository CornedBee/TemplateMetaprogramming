// Still use the old dispatch machinery. We just immediately deduce the type
// being requested.
template <typename Fn>
detail::TypeCase<detail::DeducedArgument<Fn>, Fn> typeCase_(Fn fn)
{
  return detail::TypeCase<detail::DeducedArgument<Fn>, Fn>(fn);
}

  // Decay turns const T& and T& into T
  template <typename Fn>
  using DeducedArgument = std::decay_t<FirstArgument<Fn>>;

  template <typename Fn>
  using FirstArgument = typename FirstArgumentImpl<Fn>::type;

  // The base case is used if Fn is not a function. Assume it is a class
  // with a call operator.
  template <typename Fn>
  struct FirstArgumentImpl
  {
    // Deduce the first argument of the call operator instead. This only works
    // if there is only one call operator, and it's not a template. So this
    // doesn't work with generic lambdas.
    using type = typename FirstArgumentImpl<decltype(&Fn::operator())>::type;
  };

  // Deduce for function pointers, ...
  template <typename R, typename Arg, typename... Args>
  struct FirstArgumentImpl<R (*)(Arg, Args...)>
  {
    using type = Arg;
  };
  // ... raw function types, ...
  template <typename R, typename Arg, typename... Args>
  struct FirstArgumentImpl<R(Arg, Args...)>
  {
    using type = Arg;
  };
  // ... pointers to member functions, ...
  template <typename R, typename C, typename Arg, typename... Args>
  struct FirstArgumentImpl<R (C::*)(Arg, Args...)>
  {
    using type = Arg;
  };
  // ... and pointers to const member functions.
  template <typename R, typename C, typename Arg, typename... Args>
  struct FirstArgumentImpl<R (C::*)(Arg, Args...) const>
  {
    using type = Arg;
  };
