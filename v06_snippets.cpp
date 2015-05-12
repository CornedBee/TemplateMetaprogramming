    bool supports(const std::type_info&) const { return false; }

template <typename Result, typename... Cases>
Result switchAny(boost::any& a, Cases... cases)
{
  return detail::switchAnyImpl<Result>(a, a.type(),
    detail::pickDefault<Result>(cases...), cases...);
}
template <typename Result, typename... Cases>
Result switchAny(const boost::any& a, Cases... cases)
{
  return detail::switchAnyImpl<Result>(a, a.type(),
    detail::pickDefault<Result>(cases...), cases...);
}

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

  template <typename Result, typename Case, typename... Cases>
  auto pickDefault(Case, Cases... cases)
  {
    return pickDefault<Result>(cases...);
  }
    -> decltype(pickDefault<Result>(cases...))

  template <typename Result, typename Fn, typename... Cases>
  auto pickDefault(DefaultCase<Fn> d, Cases...)
  {
    return d;
  }
    -> decltype(d)

  template <typename Result>
  struct NoDefault
  {
    Result operator ()(const boost::any&) {
      assert(false && "No case matched");
      std::terminate();
    }
  };

  template <typename Result>
  auto pickDefault() {
    return DefaultCase<NoDefault<Result>>(NoDefault<Result>());
  }
  DefaultCase<NoDefault<Result>>
