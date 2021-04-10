#include "logger.h"

#include <ctime>
#include <math.h>
#include <mutex>

////////////////////////////////////////////////////////////////////////////////
//                          Lemon 3D Graphics Engine                          //
//                    COPYRIGHT (c) 2021 by ZACH GOETHEL                      //
//  ------------------------------------------------------------------------  //
//  Use freely.  This code is published under the MIT permissive open-source  //
//  license.  For more information, see the license file included with this   //
//  repository.  Good luck, and enjoy!                                        //
//  ------------------------------------------------------------------------  //
////////////////////////////////////////////////////////////////////////////////

// The default coloration and pattern of a log message
#define LOGGER_PATTERN std::string("\033[0;37m%d{yyyy-MM-dd} %t{hh:mm:ss}\033\
[1;36m[%5p\033[1;36m][\033[1;37m%-15c\033[1;36m]\033[0;37m: %m%n\033[0m")

namespace lemon
{
	std::mutex& _log_mutex()
	{
		static std::mutex mutex;
		return mutex;
	}

	logger::logger(std::string pattern, std::string name)
	{
		this->pattern = pattern;
		this->name = name;
	}

	// Overloaded constructor with default pattern
	logger::logger(std::string name) : logger::logger(LOGGER_PATTERN, name)
	{ }

	/**
	 * @brief Convenience method for printing justified text.
	 */
	void print(std::string message, int width, bool left_justify)
	{
		// If left justified, print the message first
		if (!left_justify)
			std::cout << message.substr(0, width == 0 ? message.length() : width);
		// Print spacing for justification offset
		for (int i = 0; i < width - (int)message.length(); i++)
			std::cout << " ";
		// If left justified, print the message last
		if (left_justify)
			std::cout << message.substr(0, width == 0 ? message.length() : width);
	}

	void logger::log(std::string level, std::string level_color, std::string message)
	{
		// Lock mutex in order to synchronize logging
		std::lock_guard<std::mutex> lock(_log_mutex());

		// Convert to C strings and capture time
		auto pattern_c = pattern.c_str();
		auto now_c = time(nullptr);
		// Track flags for parsing log pattern
		bool within_brackets = false;
		bool within_token = false;
		// Track flags for text justification state
		bool is_left_justify = false;
		// Track powers of ten for number printing
		int num_digits_accum = 0;

		// Localize the date/time
		auto date_time = localtime(&now_c);

		// Store the last command character and numerical digit
		char prev_command = ' ';
		char prev_digit = ' ';
		// Track number of digits for date format
		int num_digit_count = 0;

		// Iterate through the pattern string
		//TODO ADD ADDITIONAL DOCUMENTATION
		for (unsigned int i = 0; i < pattern.length(); i++)
		{
			char c = pattern_c[i];

			if (within_brackets)
			{
				if (c == prev_digit)
					num_digit_count++;
				else
				{
					num_digit_count = 0;

					now_c = time(nullptr);
					date_time = localtime(&now_c);

					date_time->tm_year += 1900;
					date_time->tm_mon += 1;
				}

				switch (c)
				{
				case 'y':
					std::cout << date_time->tm_year / (int)pow(10, 3 - num_digit_count);
					date_time->tm_year %= (int)pow(10, 3 - num_digit_count);
					break;
				case 'M':
					std::cout << date_time->tm_mon / (int)pow(10, 1 - num_digit_count);
					date_time->tm_mon %= (int)pow(10, 1 - num_digit_count);
					break;
				case 'd':
					std::cout << date_time->tm_mday / (int)pow(10, 1 - num_digit_count);
					date_time->tm_mday %= (int)pow(10, 1 - num_digit_count);
					break;

				case 'h':
					std::cout << date_time->tm_hour / (int)pow(10, 1 - num_digit_count);
					date_time->tm_hour %= (int)pow(10, 1 - num_digit_count);
					break;
				case 'm':
					std::cout << date_time->tm_min / (int)pow(10, 1 - num_digit_count);
					date_time->tm_min %= (int)pow(10, 1 - num_digit_count);
					break;
				case 's':
					std::cout << date_time->tm_sec / (int)pow(10, 1 - num_digit_count);
					date_time->tm_sec %= (int)pow(10, 1 - num_digit_count);
					break;

				case '}' :
					within_brackets = false;
					within_token = false;
					break;

				default:
					std::cout << c;
				}

				prev_digit = c;
			} else if (within_token)
			{
				switch (c)
				{
				case '{':
					within_brackets = true;
					break;
			
				case 'd':
				case 't':
					break;

				case 'm':
					print(message, num_digits_accum, is_left_justify);
					within_token = false;
					break;
				case 'c':
					print(name, num_digits_accum, is_left_justify);
					within_token = false;
					break;
				case 'n':
					std::cout << std::endl;
					within_token = false;
					break;
				case 'p':
					std::cout << level_color;
					print(level, num_digits_accum, is_left_justify);
					within_token = false;
					break;

				case '-':
					is_left_justify = true;
					break;

				case '0':
				case '1':
				case '2':
				case '3':
				case '4':
				case '5':
				case '6':
				case '7':
				case '8':
				case '9':
					num_digits_accum *= 10;
					num_digits_accum += c - '0';
					break;

				default:
					within_token = false;
				}

				prev_command = c;
			} else
			{
				switch (c)
				{
				case '%':
					within_token = true;
					is_left_justify = false;
					num_digits_accum = 0;
					break;

				default:
					std::cout << c;
				}
			}
		}
	}

	void logger::debug(std::string message)
	{
		this->log("DEBUG", "\033[1;34m", message);
	}

	void logger::info(std::string message)
	{
		this->log("INFO", "\033[1;32m", message);
	}

	void logger::warn(std::string message)
	{
		this->log("WARN", "\033[1;33m", message);
	}

	void logger::error(std::string message)
	{
		this->log("ERROR", "\033[1;31m", message);
	}
}
