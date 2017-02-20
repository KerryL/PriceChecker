// File:  priceCheckerApp.cpp
// Date:  2/17/2017
// Auth:  K. Loux
// Desc:  Application for checking prices from web sites.  Includes application
//        entry point.

// Local headers
#include "priceCheckerApp.h"
#include "priceCheckerConfigFile.h"
#include "priceChecker.h"
#include "amazonPriceChecker.h"

// Standard C++ headers
#include <iostream>
#include <sstream>
#include <iomanip>
#include <ctime>

int main(int argc, char *argv[])
{
	if (argc == 2)
	{
		PriceCheckerApp app;
		return app.Execute(argv[1]);
	}
	else
		PriceCheckerApp::ShowUsage(argv[0]);

	return 1;
}

int PriceCheckerApp::Execute(const std::string& configFilePath)
{
	PriceCheckerConfigFile configFile;
	if (!configFile.ReadConfiguration(configFilePath))
		return 1;

	std::ostringstream historyToAdd;
	time_t now(std::time(nullptr));
	struct tm localNow(*std::localtime(&now));
	historyToAdd << std::put_time(&localNow, "%d-%m-%Y %H:%M") << ",";

	RegisterPriceCheckers();
	PriceChecker::Initialize();

	for (const auto& target : configFile.configuration.targets)
	{
		std::unique_ptr<PriceChecker> checker;
		checker = GetPriceChecker(target);
		if (!checker)
		{
			std::cerr << "Failed to determine appropriate checker for '" << target << "'\n";
			PriceChecker::Cleanup();
			return 1;
		}

		const double price(checker->GetCurrentPrice());
		if (price == PriceChecker::errorPriceCode)
		{
			std::cerr << "Failed to extract price from page for '" << target << "'\n";
			PriceChecker::Cleanup();
			return 1;
		}

		historyToAdd << price << ",";
	}

	const bool needsHeadings(!FileExists(configFile.configuration.priceHistoryFilePath));
	if (!needsHeadings)
	{
		if (!AdjustColumnHeadings(configFile.configuration.priceHistoryFilePath, configFile.configuration.targets))
			return 1;
	}

	std::ofstream historyFile(configFile.configuration.priceHistoryFilePath.c_str(), std::ios::app);
	if (!historyFile.is_open() || !historyFile.good())
	{
		std::cerr << "Failed to open '" << configFile.configuration.priceHistoryFilePath << "' for output\n";
		return 1;
	}

	if (needsHeadings)
		WriteColumnHeadings(historyFile, configFile.configuration.targets);

	historyFile << historyToAdd.str() << std::endl;

	PriceChecker::Cleanup();
	return 0;
}

void PriceCheckerApp::WriteColumnHeadings(std::ofstream& file,
	const std::vector<std::string>& targets) const
{
	file << "Timestamp,";
	for (const auto& target : targets)
		file << target << ",";
	file << "\n";
}

bool PriceCheckerApp::FileExists(const std::string& fileName)
{
	std::ifstream file(fileName.c_str());
	return file.is_open() && file.good();
}

bool PriceCheckerApp::AdjustColumnHeadings(const std::string& fileName,
	const std::vector<std::string>& targets) const
{
	std::ifstream inFile(fileName.c_str());
	if (!inFile.is_open() || !inFile.good())
	{
		std::cerr << "Failed to open '" << fileName << "' to check column headings\n";
		return false;
	}

	std::string line;
	if (!std::getline(inFile, line))
	{
		std::cerr << "Failed to read heading row from '" << fileName << "'\n";
		return false;
	}

	// TODO:  check that all of the same targets are listed and in the same order.
	// If they are, return true here.
	// If they are not, continue reading and store results.  Then, re-order, pad and/or determine padding necessary for new checks.

	inFile.close();

	// TODO:  Implement

	return true;
}

void PriceCheckerApp::RegisterPriceCheckers()
{
	PriceCheckerType checkerType;

	checkerType.checkFunction = AmazonPriceChecker::IsThisType;
	checkerType.creationFunction = AmazonPriceChecker::CreateNew;
	checkerTypes.push_back(checkerType);
}

std::unique_ptr<PriceChecker> PriceCheckerApp::GetPriceChecker(const std::string& target) const
{
	for (const auto& checkerType : checkerTypes)
	{
		if (checkerType.checkFunction(target))
			return checkerType.creationFunction(target);
	}

	return std::unique_ptr<PriceChecker>();
}

void PriceCheckerApp::ShowUsage(const std::string& calledAs)
{
	std::cout << "Usage:  " << calledAs << " configFilePath" << std::endl;
}
