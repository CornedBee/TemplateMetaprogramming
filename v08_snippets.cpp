
template <typename Fn>
detail::TypeCase<detail::DeducedArgument<Fn>, Fn> typeCase_(Fn fn)
{
  return detail::TypeCase<detail::DeducedArgument<Fn>, Fn>(fn);
}

  template <typename Fn>
  using DeducedArgument = std::decay_t<FirstArgument<Fn>>;

  template <typename Fn>
  using FirstArgument = typename FirstArgumentImpl<Fn>::type;

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
  template <typename R, typename C, typename Arg, typename... Args>
  struct FirstArgumentImpl<R (__cdecl C::*)(Arg, Args...) const>
  {
    using type = Arg;
  };
