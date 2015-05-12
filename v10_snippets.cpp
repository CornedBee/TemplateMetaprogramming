  template <typename C>
  auto asCase(C c)
    -> std::enable_if_t<!IsCallable<C()>::value,
      TypeCase<DeducedArgument<C>, C>>
  {
    return TypeCase<DeducedArgument<C>, C>(c);
  }

  template <typename C, typename = std::enable_if_t<!IsCallable<C()>::value>>
  auto asCase(C c)
  {
    return TypeCase<DeducedArgument<C>, C>(c);
  }

  struct AnyArg
  {
    template <typename T>
    operator T&&() const;
    template <typename T>
    operator T&() const;
  };

  template <typename C>
  auto asCase(C c)
    -> std::enable_if_t<IsCallable<C(AnyArg)>::value,
      TypeCase<DeducedArgument<C>, C>>
  {
    return TypeCase<DeducedArgument<C>, C>(c);
  }

  template <bool Condition, typename LazyResult>
  struct LazyEnableIfImpl {};
  template <typename LazyResult>
  struct LazyEnableIfImpl<true, LazyResult>
  {
    using type = typename LazyResult::type;
  };
  template <bool Condition, typename LazyResult>
  using LazyEnableIf = typename LazyEnableIfImpl<Condition, LazyResult>::type;

  template <typename Fn>
  struct MakeDeducedTypeCase
  {
    using type = TypeCase<DeducedArgument<Fn>, Fn>;
  };

  template <typename C>
  auto asCase(C c)
    -> LazyEnableIf<IsCallable<C(AnyArg)>::value,
      MakeDeducedTypeCase<C>>
  {
    return TypeCase<DeducedArgument<C>, C>(c);
  }

  template <typename C>
  auto asCase(C c)
    -> typename std::enable_if_t<IsCallable<C(AnyArg)>::value,
      MakeDeducedTypeCase<C>>::type
  {
    return TypeCase<DeducedArgument<C>, C>(c);
  }
