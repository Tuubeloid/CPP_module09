#include "BitcoinExchange.hpp"

BitcoinExchange::BitcoinExchange()
{
}

BitcoinExchange &BitcoinExchange::operator=(const BitcoinExchange &other)
{
	if (this != &other)
	{
		database = other.database;
	}
	return *this;
}

BitcoinExchange::~BitcoinExchange()
{
}

void BitcoinExchange::readDatabase(const std::string &filename)
{
	struct stat fileStat; 
    if (stat(filename.c_str(), &fileStat) == 0) 
    {
        if (S_ISDIR(fileStat.st_mode)) 
        {
            std::cerr << "Error: the specified database is a directory, not a file." << std::endl;
            return;
        }
    } else 
    {
        std::cerr << "Error: could not stat the file." << std::endl;
        return;
    }
	std::ifstream file(filename);
	if (!file.is_open())
		throw std::runtime_error("Error 2: could not open file.");
	std::string line;
	std::getline(file, line);	
	while (std::getline(file, line))
	{
		std::istringstream iss(line);
		std::string date;
		double value;
		std::getline(iss, date, ',');
		iss >> value;
		database[date] = value;
	}
	file.close();
}

bool BitcoinExchange::isValidDateFormat(const std::string& date) {
	if (date.length() != 10 || date[4] != '-' || date[7] != '-')
		return false;

	for (size_t i = 0; i < date.length(); ++i) {
		if (i == 4 || i == 7) continue;
		if (!std::isdigit(date[i]))
			return false;
	}
	return true;
}

bool BitcoinExchange::isValidDateValue(const std::string& date) {
	int year = std::atoi(date.substr(0, 4).c_str());
	int month = std::atoi(date.substr(5, 2).c_str());
	int day = std::atoi(date.substr(8, 2).c_str());

	time_t t = time(0);
	tm* now = localtime(&t);
	int currentYear = now->tm_year + 1900;

	if (year < 0 || year > currentYear)
		return false;

	if (month < 1 || month > 12)
		return false;

	int daysInMonth[] = {
		31, // January
		28, // February (can be 29 if leap year)
		31, // March
		30, // April
		31, // May
		30, // June
		31, // July
		31, // August
		30, // September
		31, // October
		30, // November
		31  // December
	};

	bool isLeapYear = (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
	if (isLeapYear && month == 2)
		daysInMonth[1] = 29;

	if (day < 1 || day > daysInMonth[month - 1])
		return false;

	return true;
}

std::string BitcoinExchange::trim(const std::string& str)
{
	size_t start = str.find_first_not_of(" \t");
	size_t end = str.find_last_not_of(" \t");

	if (start == std::string::npos)
		return "";

	return str.substr(start, end - start + 1);
}

std::string BitcoinExchange::findClosestDate(const std::string& date)
{
	auto it = database.find(date);
	if (it != database.end())
		return it->first;

	auto upper = database.upper_bound(date);
	if (upper == database.begin())
		return database.begin()->first;

	auto lower = std::prev(upper);
	if (upper == database.end())
		return lower->first;

	if (upper->first == date)
		return upper->first;

	return lower->first;
}

void BitcoinExchange::readAndProcessInput(const std::string& filename)
{
	struct stat fileStat; 
    if (stat(filename.c_str(), &fileStat) == 0) 
    {
        if (S_ISDIR(fileStat.st_mode)) 
        {
            std::cerr << "Error: the specified input is a directory, not a file." << std::endl;
            return;
        }
    } else 
    {
        std::cerr << "Error: could not stat the file." << std::endl;
        return;
    }
	std::ifstream file(filename);
	if (!file.is_open())
		throw std::runtime_error("Error: could not open input file.");
	int i = 0;
	std::string line;
	while (std::getline(file, line))
	{
		if (i == 0)
		{
			// add check if the first line is header in form: "date | value"
			trim(line);
			if (line != "date | value")
				std::cerr << "Error: bad or missing header." << std::endl;
			else
				std::getline(file, line);
			++i;
		}
		std::istringstream iss(line);
		std::string dateStr;
		double value;
		if (!std::getline(iss, dateStr, '|'))
		{
			std::cerr << "Error: bad input => " << line << std::endl;
			continue;
		}
		dateStr = trim(dateStr);
		if (!(iss >> value))
		{
			std::cerr << "Error: bad input => " << line << std::endl;
			continue;
		}
		if (!isValidDateFormat(dateStr) || !isValidDateValue(dateStr))
		{
			std::cerr << "Error: bad input => " << dateStr << std::endl;
			continue;
		}
	    std::string remaining;
        if (iss >> remaining) 
        {
            std::cerr << "Error: bad input => " << line << std::endl;
            continue;
        }
		if (value < 0)
		{
			std::cerr << "Error: not a positive number." << std::endl;
			continue;
		}
		if (value > 1000)
		{
			std::cerr << "Error: too large a number." << std::endl;
			continue;
		}
		std::string closestDate = findClosestDate(dateStr);
		if (closestDate.empty())
		{
			std::cerr << "Error: no matching date found." << std::endl;
			continue;
		}
		std::cout << closestDate << " => " << value << " = " << value * database[closestDate] << std::endl;
	}
	file.close();
}

void BitcoinExchange::run(const std::string &filename)
{
	readDatabase("data.csv");
	readAndProcessInput(filename);
}
