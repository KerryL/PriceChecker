// File:  priceCheckerApp.h
// Date:  2/17/2017
// Auth:  K. Loux
// Desc:  Application for checking prices from web sites.

#ifndef PRICE_CHECKER_APP_H_
#define PRICE_CHECKER_APP_H_

// Standard C++ headers
#include <string>
#include <fstream>
#include <memory>
#include <vector>

// Local forward declarations
class PriceChecker;

class PriceCheckerApp
{
public:
	int Execute(const std::string& configFilePath);

	static void ShowUsage(const std::string& calledAs);

private:
	static const double skippedPriceCode;
	std::unique_ptr<PriceChecker> GetPriceChecker(const std::string& target) const;

	typedef bool (*TypeCheckFunction)(const std::string&);
	typedef std::unique_ptr<PriceChecker> (*CheckerCreationFunction)(const std::string&);

	struct PriceCheckerType
	{
		TypeCheckFunction checkFunction;
		CheckerCreationFunction creationFunction;
	};

	void RegisterPriceCheckers();
	std::vector<PriceCheckerType> checkerTypes;

	void WriteColumnHeadings(std::ofstream& file, const std::vector<std::string>& targets) const;
	bool AdjustColumnHeadings(const std::string& fileName, const std::vector<std::string>& targets,
		unsigned int& padColumnCount) const;

	static bool FileExists(const std::string& fileName);
	static std::vector<std::string> SplitBy(const std::string& s, const char& delimiter);
};

#endif// PRICE_CHECKER_APP_H_