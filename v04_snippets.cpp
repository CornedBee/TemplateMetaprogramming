    std::result_of_t<Fn()> call(boost::any&) { return fn(); }

    std::result_of_t<Fn()> call(boost::any&) { return fn(); }

    std::result_of_t<Fn(T&)>
    auto call(boost::any& a) {
      return fn(*boost::unsafe_any_cast<T>(&a));
    }

  template <typename Result, typename Case, typename... Cases>
  Result switchAnyImpl(boost::any& a, const std::type_info& t,
                       Case c, Cases... cases) {
    if (c.supports(t))
      return c.call(a);
    return switchAnyImpl<Result>(a, t, cases...);
  }
