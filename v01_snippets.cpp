
// Just get things to compile
template <typename Result, typename... Cases>
Result switchAny(boost::any& a, Cases... cases)
{
  return 0;
}

template <typename Fn>
int defaultCase(Fn fn)
{
  return 0;
}


template <typename Fn>
detail::DefaultCase<Fn> defaultCase(Fn fn)
{
  return detail::DefaultCase<Fn>(fn);
}


namespace detail
{

  template <typename Fn>
  class DefaultCase
  {
  public:
    explicit DefaultCase(Fn fn) : fn(fn) {}

    Fn getFn() const { return fn; }

  private:
    Fn fn;
  };

}

// Need to try out the various cases. Most straight-forward way to do this
// is a recrusive function.
template <typename Result, typename... Cases>
Result switchAny(boost::any& a, Cases... cases)
{
  return detail::switchAnyImpl<Result>(a, cases...);
}

  // No cases left. This is an incomplete switch. Assert and terminate.
  template <typename Result>
  Result switchAnyImpl(boost::any& a) {
    assert(false && "No case matched.");
    std::terminate();
  }

  // There is at least one case. For our current test case, we'll just get that
  // case's function and call it.
  template <typename Result, typename Case, typename... Cases>
  Result switchAnyImpl(boost::any& a, Case c, Cases... cases) {
    return c.getFn()();
  }



