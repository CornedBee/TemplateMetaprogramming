
  // Everything not callable without arguments will be wrapped in a type case.
  // The assumption is that it is callable with one argument.
  template <typename C>
  auto asCase(C c)
    -> std::enable_if_t<!IsCallable<C()>::value,
      TypeCase<DeducedArgument<C>, C>>
  {
    return TypeCase<DeducedArgument<C>, C>(c);
  }

  // Since this doesn't try to get the argument type of C in the signature, it
  // won't instantiate DeducedArgument until this overload is actually chosen.
  // But the specialized versions for the case classes are preferred.
  // But I still need a solution for MSVC 2013.
  template <typename C, typename = std::enable_if_t<!IsCallable<C()>::value>>
  auto asCase(C c)
  {
    return TypeCase<DeducedArgument<C>, C>(c);
  }

  // This can convert to absolutely everything.
  struct AnyArg
  {
    template <typename T>
    operator T&&() const;
    template <typename T>
    operator T&() const;
  };

  // Try to filter out better. Instead of accepting anything not callable with
  // no arguments, accept things callable with one arguments.
  template <typename C>
  auto asCase(C c)
    -> std::enable_if_t<IsCallable<C(AnyArg)>::value,
      TypeCase<DeducedArgument<C>, C>>
  {
    return TypeCase<DeducedArgument<C>, C>(c);
  }

  // This is insufficient. Even though the condition is false, the result type
  // is still required.

  // Lazy enable_if uses LazyResult::type as the result type, but it only looks
  // at this type if the condition is true.
  template <bool Condition, typename LazyResult>
  struct LazyEnableIfImpl {};
  template <typename LazyResult>
  struct LazyEnableIfImpl<true, LazyResult>
  {
    using type = typename LazyResult::type;
  };
  template <bool Condition, typename LazyResult>
  using LazyEnableIf = typename LazyEnableIfImpl<Condition, LazyResult>::type;

  // This is the wrapper to hide the DeducedArgument usage.
  template <typename Fn>
  struct MakeDeducedTypeCase
  {
    using type = TypeCase<DeducedArgument<Fn>, Fn>;
  };

  // This version finally works.
  template <typename C>
  auto asCase(C c)
    -> LazyEnableIf<IsCallable<C(AnyArg)>::value,
      MakeDeducedTypeCase<C>>
  {
    return TypeCase<DeducedArgument<C>, C>(c);
  }

  // Lazy enable_if isn't really necessary, except this version triggers some
  // bug in MSVC 2013.
  template <typename C>
  auto asCase(C c)
    -> typename std::enable_if_t<IsCallable<C(AnyArg)>::value,
      MakeDeducedTypeCase<C>>::type
  {
    return TypeCase<DeducedArgument<C>, C>(c);
  }
