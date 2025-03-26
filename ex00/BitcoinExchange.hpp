#pragma once

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <map>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <exception>
#include <sys/stat.h>
#include <regex>
#include <ctime>
#include <climits>
#include <cstdlib>
#include <cctype>
#include <vector>

class BitcoinExchange
{
public:
	BitcoinExchange();
	BitcoinExchange &operator=(const BitcoinExchange &other);
	~BitcoinExchange();
	void readDatabase(const std::string &filename);
	void readAndProcessInput(const std::string &filename);
	void run(const std::string &filename);
	bool isValidDateValue(const std::string& date);
	bool isValidDateFormat(const std::string& date);
	std::string findClosestDate(const std::string& date);
	std::string trim(const std::string& str);

private:
	std::map<std::string, double> database;
};