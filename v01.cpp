#include "fluff.hpp"

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



TEST(SwitchAny, usesDefaultCase)
{
	boost::any a;
	int result = switchAny<int>(a,
		defaultCase([] { return 1; })
	);
	EXPECT_EQ(1, result);
}
