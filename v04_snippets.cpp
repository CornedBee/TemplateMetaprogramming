
template <typename T, typename Fn>
detail::TypeCase<T, Fn> typeCase(Fn fn)
{
  return detail::TypeCase<T, Fn>(fn);
}

  template <typename T, typename Fn>
  class TypeCase
  {
  public:
    explicit TypeCase(Fn fn) : fn(std::move(fn)) {}

    // Supports the specific type
    bool supports(const std::type_info& t) const { return t == typeid(T); }
    Fn getFn() const { return fn; }

  private:
    Fn fn;
  };

// But this fails to compile.

    // Use C++14 return type deduction. If that's not supported, std::result_of
    // can help.
    auto call(boost::any&) { return fn(); }

    std::result_of_t<Fn()>

    // unsafe_any_cast is like a static_cast of a void pointer.
    // It's not an official part of the Boost.Any API, but used here as a
    // performance optimization.
    auto call(boost::any& a) {
      return fn(*boost::unsafe_any_cast<T>(&a));
    }

    std::result_of_t<Fn(T&)>

  // Now we ask the case object to call its function instead of doing it
  // ourselves.
  template <typename Result, typename Case, typename... Cases>
  Result switchAnyImpl(boost::any& a, const std::type_info& t,
                       Case c, Cases... cases) {
    if (c.supports(t))
      return c.call(a);
    return switchAnyImpl<Result>(a, t, cases...);
  }
