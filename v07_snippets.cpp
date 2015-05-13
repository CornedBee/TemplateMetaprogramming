// Default the result to void, so that it doesn't need to be specified
// explicitly.
template <typename Result = void, typename... Cases>

    // The function used for a default case doesn't receive an argument.
    Result operator ()() {

  // Specialize for void to not abort. A missing default case is OK here.
  template <>
  struct NoDefault<void>
  {
    void operator ()() const {}
  };
