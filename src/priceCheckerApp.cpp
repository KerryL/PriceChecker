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
#include <algorithm>
#include <iterator>

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

const double PriceCheckerApp::skippedPriceCode(-3.0);

int PriceCheckerApp::Execute(const std::string& configFilePath)
{
	PriceCheckerConfigFile configFile;
	if (!configFile.ReadConfiguration(configFilePath))
		return 1;

	std::ostringstream historyToAdd;
	time_t now(std::time(nullptr));
	struct tm localNow(*std::localtime(&now));
	historyToAdd << std::put_time(&localNow, "%d-%m-%Y %H:%M") << ',';

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
	unsigned int padColumnCount(0);
	if (!needsHeadings)
	{
		if (!AdjustColumnHeadings(configFile.configuration.priceHistoryFilePath,
			configFile.configuration.targets, padColumnCount))
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

	while (padColumnCount > 0)
	{
		historyToAdd << skippedPriceCode << ',';
		--padColumnCount;
	}

	historyFile << historyToAdd.str() << std::endl;

	PriceChecker::Cleanup();
	return 0;
}

void PriceCheckerApp::WriteColumnHeadings(std::ofstream& file,
	const std::vector<std::string>& targets) const
{
	file << "Timestamp,";
	for (const auto& target : targets)
		file << target << ',';
	file << '\n';
}

bool PriceCheckerApp::FileExists(const std::string& fileName)
{
	std::ifstream file(fileName.c_str());
	return file.is_open() && file.good();
}

bool PriceCheckerApp::AdjustColumnHeadings(const std::string& fileName,
	const std::vector<std::string>& targets, unsigned int& padColumnCount) const
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

	std::vector<std::string> headings(SplitBy(line, ','));
	if (headings.size() == targets.size() + 1 &&
		std::mismatch(headings.begin() + 1, headings.end(), targets.begin(), targets.end()).first == headings.end())
		return true;

	std::vector<std::pair<unsigned int, std::vector<std::string>>> data;
	unsigned int i(0);
	for (const auto& heading : headings)
		data.push_back(std::make_pair(headings.size() + i++, std::vector<std::string>(1, heading)));
	data[0].first = 0;

	// TODO:  Does not properly handle repeated targets (i.e. same target twice) - should we check for this in config file class?
	i = 1;
	for (const auto& target : targets)
	{
		bool found(false);
		for (auto& entry : data)
		{
			if (entry.second[0].compare(target) == 0)
			{
				entry.first = i;
				found = true;
				break;
			}
		}

		if (!found)
			data.push_back(std::make_pair(i, std::vector<std::string>(1, target)));

		++i;
	}

	std::ostringstream skippedCode;
	skippedCode << skippedPriceCode;
	while (std::getline(inFile, line))
	{
		std::vector<std::string> items(SplitBy(line, ','));
		for (i = 0; i < items.size(); ++i)
			data[i].second.push_back(items[i]);

		for (; i < data.size(); ++i)
			data[i].second.push_back(skippedCode.str());
	}

	inFile.close();

	std::sort(data.begin(), data.end());

	std::ofstream outFile(fileName.c_str());
	if (!outFile.is_open() || !outFile.good())
	{
		std::cerr << "Failed to open '" << fileName << "' for re-write\n";
		return false;
	}

	for (i = 0; i < data[0].second.size(); ++i)
	{
		unsigned int j;
		for (j = 0; j < data.size(); ++j)
			outFile << data[j].second[i] << ',';
		outFile << '\n';
	}

	padColumnCount = data.size() - targets.size() - 1;

	return true;
}

std::vector<std::string> PriceCheckerApp::SplitBy(const std::string& s, const char& delimiter)
{
	std::vector<std::string> tokens;
	std::istringstream ss;
	ss.str(s);
	std::string token;
	while (std::getline(ss, token, delimiter))
		tokens.push_back(token);

	return tokens;
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
