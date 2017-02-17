// File:  priceCheckerConfig.h
// Date:  2/17/2017
// Auth:  K. Loux
// Desc:  Configuration for checking internet prices.

#ifndef PRICE_CHECKER_CONFIG_H_
#define PRICE_CHECKER_CONFIG_H_

// Standard C++ headers
#include <vector>
#include <string>

struct PriceCheckerConfig
{
	std::vector<std::string> targets;
	std::string priceHistoryFilePath;
};

#endif// PRICE_CHECKER_CONFIG_H_