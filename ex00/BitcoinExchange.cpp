/*You have to create a program which outputs the value of a certain amount of bitcoin
on a certain date.
This program must use a database in csv format which will represent bitcoin price
over time. This database is provided with this subject.
The program will take as input a second database, storing the different prices/dates
to evaluate.
Your program must respect these rules:
• The program name is btc.
• Your program must take a file as an argument.
• Each line in this file must use the following format: "date | value".
• A valid date will always be in the following format: Year-Month-Day.
• A valid value must be either a float or a positive integer, between 0 and 1000.
Here is an example of an input.txt file:
$> head input.txt
date | value
2011-01-03 | 3
2011-01-03 | 2
2011-01-03 | 1
2011-01-03 | 1.2
2011-01-09 | 1
2012-01-11 | -1
2001-42-42
2012-01-11 | 1
2012-01-11 | 2147483648
$>
Your program will use the value in your input file.
Your program should display on the standard output the result of the value multiplied
by the exchange rate according to the date indicated in your database.
If the date used in the input does not exist in your DB then you
must use the closest date contained in your DB. Be careful to use the
lower date and not the upper one.
The following is an example of the program’s use.
$> ./btc
Error: could not open file.
$> ./btc input.txt
2011-01-03 => 3 = 0.9
2011-01-03 => 2 = 0.6
2011-01-03 => 1 = 0.3
2011-01-03 => 1.2 = 0.36
2011-01-09 => 1 = 0.32
Error: not a positive number.
Error: bad input => 2001-42-42
2012-01-11 => 1 = 7.1
Error: too large a number.
$>
Warning: The container(s) you use to validate this exercise will no
longer be usable for the rest of this module.

*/

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
	std::ifstream
		file(filename);
	if (!file.is_open())
	{
		throw std::runtime_error("Error 2: could not open file.");
	}
	std::string
		line;
	std::getline(file, line);	// skip header	
	while (std::getline(file, line))
	{
		std::istringstream
			iss(line);
		std::string
			date;
		double
			value;
		std::getline(iss, date, ',');
		iss >> value;
		database[date] = value;
	}
	file.close();
}

// Make checks to see if the date format is proper.
// Year can be only 0 to this year
// Month can only be 1-12
// Day can only be 1-31
// So the date is in format: 2012-01-11 | If theres any letters or other symbols or for example 2001-42-42 its 
// Error: bad input => 2001-42-42
// If value is negative:
// Error: not a positive number.
// If value is overflown:
// Error: too large a number.
void BitcoinExchange::readInput(const std::string &filename)
{
	std::ifstream
		file(filename);
	if
		(!file.is_open())
	{
		throw std::runtime_error("Error 1: could not open file.");
	}
	std::string
		line;
	std::getline(file, line);	// skip header
	while (std::getline(file, line))
	{
		std::istringstream
			iss(line);
		std::string
			date;
		double
			value;
		std::getline(iss, date, '|');
		iss >> value;
		input[date] = value;
	}
	file.close();
}

void BitcoinExchange::exchangeRate()
{
	for (auto &it : input)
	{
		auto
			upper = database.upper_bound(it.first);
		if (upper == database.begin())
		{
			result[it.first] = 0;
		}
		else
		{
			auto
				lower = std::prev(upper);
			if (upper == database.end())
			{
				result[it.first] = lower->second;
			}
			else
			{
				if (upper->first == it.first)
				{
					result[it.first] = upper->second;
				}
				else
				{
					result[it.first] = lower->second;
				}
			}
		}
	}
}

void BitcoinExchange::display()
{
	/*
    for (auto &it : result)
    {
        auto inputIt = input.find(it.first);
        
        if (inputIt == input.end())
        {
            std::cerr << "Error: No matching input found for date: " << it.first << std::endl;
            continue;
        }

        if (inputIt->second < 0)
        {
            std::cerr << "Error: not a positive number." << std::endl;
            continue;
        }
        else if (inputIt->second > 1000)
        {
            std::cerr << "Error: too large a number." << std::endl;
            continue;
        }

        // Correctly output the exchange rate calculation
        std::cout << it.first << " => " << inputIt->second << " = "
                  << std::fixed << std::setprecision(2)
                  << inputIt->second * it.second << std::endl;
    }
	*/
	for (auto &it : database)
	{
		std::cout << "Date in database: " << it.first <<  " : The rate in database: " << std::setprecision(10) << it.second << std::endl;
	}
	for (auto &it : input)
	{
		std::cout << "Date in input: " << it.first <<  " : The rate in input: " << std::setprecision(10) << it.second << std::endl;
	}
	for (auto &it : result)
	{
		std::cout << "Date in result: " << it.first <<  " : The rate in result: " << std::setprecision(10) << it.second << std::endl;
	}
}

void BitcoinExchange::run(const std::string &filename)
{
	readDatabase("data.csv");
	readInput(filename);
	exchangeRate();
	display();
}

