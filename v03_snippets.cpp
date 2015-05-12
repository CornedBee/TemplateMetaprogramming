    bool supports(const std::type_info&) const { return true; }
    bool supports(const std::type_info& t) const { return t == typeid(void); }


  template <typename Fn>
  class EmptyCase
  {
  public:
    explicit EmptyCase(Fn fn) : fn(fn) {}

    Fn getFn() const { return fn; }

  private:
    Fn fn;
  };

  template <typename Result>
  Result switchAnyImpl(boost::any& a, const std::type_info& t) {
    assert(false && "No case matched.");
    std::terminate();
  }

  template <typename Result, typename Case, typename... Cases>
  Result switchAnyImpl(boost::any& a, const std::type_info& t,
                       Case c, Cases... cases) {
    if (c.supports(t))
      return c.getFn()();
    return switchAnyImpl<Result>(a, t, cases...);
  }


template <typename Result, typename... Cases>
Result switchAny(boost::any& a, Cases... cases)
{
  return detail::switchAnyImpl<Result>(a, a.type(), cases...);
}

template <typename Fn>
detail::EmptyCase<Fn> emptyCase(Fn fn)
{
  return detail::EmptyCase<Fn>(fn);
}
