// File:  amazonPriceChecker.h
// Date:  2/17/2017
// Auth:  K. Loux
// Desc:  Class for pulling prices from Amazon pages.

#ifndef AMAZON_PRICE_CHECKER_H_
#define AMAZON_PRICE_CHECKER_H_

// Local headers
#include "priceChecker.h"

// Standard C++ headers
#include <memory>

class AmazonPriceChecker : public PriceChecker
{
public:
	AmazonPriceChecker(const std::string& target) : PriceChecker(target) {}

	// Derived classes must implement:
	static bool IsThisType(const std::string& target);
	static std::unique_ptr<PriceChecker> CreateNew(const std::string& target);

protected:
	double ExtractPrice(const std::string& rawPage) const override;

private:
	static const std::string amazonURL;
};

#endif// AMAZON_PRICE_CHECKER_H_
