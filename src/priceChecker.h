// File:  priceChecker.h
// Date:  2/17/2017
// Auth:  K. Loux
// Desc:  Abstract base class for objects that pull price data from target web
//        sites.

#ifndef PRICE_CHECKER_H_
#define PRICE_CHECKER_H_

// Standard C++ headers
#include <string>

class PriceChecker
{
public:
	PriceChecker(const std::string& target);
	virtual ~PriceChecker() = default;

	double GetCurrentPrice() const;

	// Derived classes must implement:
	//static bool IsThisType(const std::string& target);
	//static std::unique_ptr<PriceChecker> CreateNew(const std::string& target);

	static void Initialize();
	static void Cleanup();

	static const double errorPriceCode;

protected:
	virtual double ExtractPrice(const std::string& rawPage) const = 0;

private:
	const std::string target;

	static size_t HttpWriteCallback(void* dataIn, size_t size, size_t nmemb, void* dataOut);
};

#endif// PRICE_CHECKER_H_
