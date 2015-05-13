
// Lots of annyoing boilerplate.
std::string stringFromAny(const boost::any& a)
{
	if (auto* s = boost::any_cast<std::string>(&a)) {
		return *s;
	}
	if (auto* cp = boost::any_cast<const char*>(&a)) {
		return std::string(*cp);
	}
	if (auto* cp = boost::any_cast<char*>(&a)) {
		return std::string(*cp);
	}
	return std::string();
}

































// Wishful thinking. switch is a more readable way of writing if-else chains,
// but only work for integral constants. How close can we get to this?
std::string stringFromAny(const boost::any& a)
{
	switch (a)
	{
	case std::string s: return s;
	case const char* cp: return std::string(cp);
	case char* cp: return std::string(cp);
	default: return std::string();
	}
}






































// This is our first goal
std::string stringFromAny(const boost::any& a)
{
	return switchAny(a,
		typeCase<std::string>([](auto& s) { return s; }),
		typeCase<const char*>([](auto& cp) { return std::string(cp); }),
		typeCase<char*>([](auto& cp) { return std::string(cp); }),
		defaultCase([] { return std::string(); })
	);
}










































// I use MSVC 2013 at work - no generic lambdas!
// So we're back to lots of boilerplate: the type is written twice.
std::string stringFromAny(const boost::any& a)
{
	return switchAny(a,
		typeCase<std::string>([](const std::string& s) { return s; }),
		typeCase<const char*>([](const char* cp) { return std::string(cp); }),
		typeCase<char*>([](char* cp) { return std::string(cp); }),
		defaultCase([] { return std::string(); })
	);
}












































// If the type is in the lambda argument, why put it in the typeCase?
// This is our second goal.
std::string stringFromAny(const boost::any& a)
{
	return switchAny(a,
		typeCase_([](const std::string& s) { return s; }),
		typeCase_([](const char* cp) { return std::string(cp); }),
		typeCase_([](char* cp) { return std::string(cp); }),
		defaultCase([] { return std::string(); })
	);
}












































// In fact, why have typeCase in the interface at all?
// This is our final goal.
std::string stringFromAny(const boost::any& a)
{
	return switchAny(a,
		[](const std::string& s) { return s; },
		[](const char* cp) { return std::string(cp); },
		[](char* cp) { return std::string(cp); },
		default_([] { return std::string(); })
	);
}
