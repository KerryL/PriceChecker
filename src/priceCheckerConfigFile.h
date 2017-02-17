// File:  priceCheckerConfigFile.h
// Date:  2/17/2017
// Auth:  K. Loux
// Desc:  Configuration file object for price checker application.

#ifndef PRICE_CHECKER_CONFIG_FILE_H_
#define PRICE_CHECKER_CONFIG_FILE_H_

// utilities headers
#include "utilities/configFile.h"

// Local headers
#include "priceCheckerConfig.h"

class PriceCheckerConfigFile : public ConfigFile
{
public:
	PriceCheckerConfig configuration;

protected:
	void BuildConfigItems() override;
	void AssignDefaults() override;
	bool ConfigIsOK() override;
};

#endif// PRICE_CHECKER_CONFIG_FILE_H_