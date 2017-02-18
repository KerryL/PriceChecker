// File:  priceChecker.cpp
// Date:  2/17/2017
// Auth:  K. Loux
// Desc:  Abstract base class for objects that pull price data from target web
//        sites.

// Local headers
#include "priceChecker.h"

// libcurl headers
#include <curl/curl.h>

// Standard C++ headers
#include <iostream>
#include <algorithm>
#include <cctype>

PriceChecker::PriceChecker(const std::string& target) : target([target]()
{
	std::string lowerTarget(target);
	std::transform(lowerTarget.begin(), lowerTarget.end(), lowerTarget.begin(), ::tolower);
	return lowerTarget;
}())
{
}

double PriceChecker::GetCurrentPrice() const
{
	CURL *curl(curl_easy_init());
	if (!curl)
	{
		std::cerr << "Failed to obtain curl handle\n";
		return -1.0;
	}

	curl_easy_setopt(curl, CURLOPT_URL, target.c_str());
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, PriceChecker::HttpWriteCallback);

	std::string pageSource;
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*)&pageSource);
	curl_easy_setopt(curl, CURLOPT_USERAGENT, "libcurl-agent/1.0");

	CURLcode res(curl_easy_perform(curl));
	curl_easy_cleanup(curl);

	if (res != CURLE_OK)
	{
		std::cerr << "Error during GET:  " << curl_easy_strerror(res) << "\n";
		return -1.0;
	}

	return ExtractPrice(pageSource);
}

void PriceChecker::Initialize()
{
	curl_global_init(CURL_GLOBAL_DEFAULT);
}

void PriceChecker::Cleanup()
{
	curl_global_cleanup();
}

size_t PriceChecker::HttpWriteCallback(void* dataIn, size_t size, size_t nmemb, void* dataOut)
{
	size_t realSize(size * nmemb);
	std::string* data(static_cast<std::string*>(dataOut));
	std::string newData(static_cast<const char*>(dataIn), realSize);
	data->append(newData);
	return realSize;
}
