// File:  priceChecker.cpp
// Date:  2/17/2017
// Auth:  K. Loux
// Desc:  Abstract base class for objects that pull price data from target web
//        sites.

// Local headers
#include "priceChecker.h"

// Standard C++ headers
#include <algorithm>
#include <cctype>

PriceChecker::PriceChecker(const std::string& target) : target([target]()
{
	std::string lowerTarget(target);
	std::transform(lowerTarget.begin(), lowerTarget.end(), lowerTarget.begin(), std::tolower);
	return lowerTarget;
}())
{
}

double PriceChecker::GetCurrentPrice() const
{
	// Connect to web site
	// Get contents
	std::string pageSource;
	return ExtractPrice(pageSource);
}
