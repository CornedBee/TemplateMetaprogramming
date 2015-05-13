
  template <typename Fn>
  class EmptyCase
  {
  public:
    explicit EmptyCase(Fn fn) : fn(fn) {}

    Fn getFn() const { return fn; }

  private:
    Fn fn;
  };

template <typename Fn>
detail::EmptyCase<Fn> emptyCase(Fn fn)
{
  return detail::EmptyCase<Fn>(fn);
}

    // Add a member to query whether this case supports a given type.
    bool supports(const std::type_info&) const { return true; }

    // An empty any reports that it contains type void.
    bool supports(const std::type_info& t) const { return t == typeid(void); }

  template <typename Result, typename Case, typename... Cases>
  Result switchAnyImpl(boost::any& a, const std::type_info& t,
                       Case c, Cases... cases) {
    // Now ask if the case supports the contained type.
    if (c.supports(t))
      return c.getFn()();
    return switchAnyImpl<Result>(a, t, cases...);
  }

  template <typename Result>
  Result switchAnyImpl(boost::any& a, const std::type_info& t) {
    assert(false && "No case matched.");
    std::terminate();
  }

template <typename Result, typename... Cases>
Result switchAny(boost::any& a, Cases... cases)
{
  // As a performance optimization, retrieve the type only once.
  return detail::switchAnyImpl<Result>(a, a.type(), cases...);
}
