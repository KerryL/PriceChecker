// File:  amazonPriceChecker.cpp
// Date:  2/17/2017
// Auth:  K. Loux
// Desc:  Class for pulling prices from Amazon pages.

// Local headers
#include "amazonPriceChecker.h"

const std::string AmazonPriceChecker::amazonURL("www.amazon.com");

bool AmazonPriceChecker::IsThisType(const std::string& target)
{
	size_t start;
	for (start = 0; start < target.length() - amazonURL.length(); ++start)
	{
		// NOTE:  target will always be lowercase due to PriceChecker constructor
		if (target.substr(start, amazonURL.length()).compare(amazonURL) == 0)
			return true;
	}

	return false;
}

std::unique_ptr<PriceChecker> AmazonPriceChecker::CreateNew(const std::string& target)
{
	return std::make_unique<AmazonPriceChecker>(target);
}

double AmazonPriceChecker::ExtractPrice(const std::string& rawPage) const
{
	// TODO:  Implement
	return 0.0;
}
