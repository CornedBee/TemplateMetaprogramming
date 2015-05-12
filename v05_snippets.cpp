template <typename Result, typename... Cases>
Result switchAny(const boost::any& a, Cases... cases)
{
  return detail::switchAnyImpl<Result>(a, a.type(), cases...);
}

  template <typename Result, typename Any>
  Result switchAnyImpl(Any& a, const std::type_info& t) {

  template <typename Result, typename Any, typename Case, typename... Cases>
  Result switchAnyImpl(Any& a, const std::type_info& t,
                       Case c, Cases... cases) {

    template <typename Any>
    auto call(Any&) { return fn(); }

    template <typename Any>
    auto call(Any& a) {
      return fn(*boost::unsafe_any_cast<T>(&a));
    }
