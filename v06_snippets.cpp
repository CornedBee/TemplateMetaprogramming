    // Always skip over the default case in the normal search.
    bool supports(const std::type_info&) const { return false; }

template <typename Result, typename... Cases>
Result switchAny(boost::any& a, Cases... cases)
{
  // Pick out the default case or generate one, and pass it separately.
  return detail::switchAnyImpl<Result>(a, a.type(),
    detail::pickDefault<Result>(cases...), cases...);
}
template <typename Result, typename... Cases>
Result switchAny(const boost::any& a, Cases... cases)
{
  return detail::switchAnyImpl<Result>(a, a.type(),
    detail::pickDefault<Result>(cases...), cases...);
}

  // When no other case fits, use the default.
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

  // Given some random case, we keep searching. Pre-C++14 compilers need a
  // trailing return type using decltype.
  template <typename Result, typename Case, typename... Cases>
  auto pickDefault(Case, Cases... cases)
  {
    return pickDefault<Result>(cases...);
  }

    -> decltype(pickDefault<Result>(cases...))

  // If we find a default case, use it.
  template <typename Result, typename Fn, typename... Cases>
  auto pickDefault(DefaultCase<Fn> d, Cases...)
  {
    return d;
  }
    -> decltype(d)

  // If we find no default case anywhere, create one that does the assert.
  template <typename Result>
  auto pickDefault() {
    return DefaultCase<NoDefault<Result>>(NoDefault<Result>());
  }
  DefaultCase<NoDefault<Result>>

  template <typename Result>
  struct NoDefault
  {
    Result operator ()(const boost::any&) {
      assert(false && "No case matched");
      std::terminate();
    }
  };
