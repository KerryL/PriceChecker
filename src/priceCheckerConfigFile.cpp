// File:  priceCheckerConfigFile.cpp
// Date:  2/17/2017
// Auth:  K. Loux
// Desc:  Configuration file object for price checker application.

// Local headers
#include "priceCheckerConfigFile.h"

void PriceCheckerConfigFile::BuildConfigItems()
{
	AddConfigItem("TARGET", configuration.targets);
	AddConfigItem("HISTORY_FILE", configuration.priceHistoryFilePath);
}

void PriceCheckerConfigFile::AssignDefaults()
{
	configuration.priceHistoryFilePath = "priceHistory.csv";
}

bool PriceCheckerConfigFile::ConfigIsOK()
{
	bool ok(true);
	if (configuration.targets.size() == 0)
	{
		ok = false;
		outStream << "Must specify at least one " << GetKey(configuration.targets) << "\n";
	}

	if (configuration.priceHistoryFilePath.empty())
	{
		ok = false;
		outStream << GetKey(configuration.priceHistoryFilePath) << " must not be empty\n";
	}

	return ok;
}
