
// Cheating: just reuse the same underlying type.
template <typename Fn>
detail::DefaultCase<Fn> emptyCase(Fn fn)
{
  return detail::DefaultCase<Fn>(fn);
}
