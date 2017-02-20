// File:  amazonPriceChecker.cpp
// Date:  2/17/2017
// Auth:  K. Loux
// Desc:  Class for pulling prices from Amazon pages.

// Local headers
#include "amazonPriceChecker.h"

// Standard C++ headers
#include <iostream>
#include <sstream>

const std::string AmazonPriceChecker::amazonURL("www.amazon.com");
const std::string AmazonPriceChecker::startTag("<span id=\"priceblock_ourprice\" class=\"a-size-medium a-color-price\">");
const std::string AmazonPriceChecker::unavailableString("currently unavailable");
const std::string AmazonPriceChecker::endTag("</span");

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
	//std::cout << rawPage << std::endl;
	const size_t unavailableStart(rawPage.find(unavailableString));
	if (unavailableStart != std::string::npos)
		return -2.0;// Users will to know that -2 means unavailable

	const size_t start(rawPage.find(startTag));
	if (start == std::string::npos)
	{
		std::cerr << "Failed to parse page source\n";
		return errorPriceCode;
	}

	const size_t end(rawPage.substr(start).find(endTag));
	if (end == std::string::npos)
	{
		std::cerr << "Failed to parse page source\n";
		return errorPriceCode;
	}

	std::istringstream ss;
	// Additional +1/-1 is to drop the leading dollar sign
	ss.str(rawPage.substr(start + startTag.length() + 1, end - startTag.length() - 1));
	double price;
	if ((ss >> price).fail())
	{
		std::cerr << "Failed to convert price to number\n";
		return errorPriceCode;
	}

	return price;
}
