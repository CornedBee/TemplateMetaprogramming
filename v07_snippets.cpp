template <typename Result = void, typename... Cases>

    Result operator ()() {

  template <>
  struct NoDefault<void>
  {
    void operator ()() const {}
  };
